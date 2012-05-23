/*
 *
 *  Copyright(C) 2010  Mattias Runge
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <signal.h>
#include <sys/resource.h>

#include <boost/program_options.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/locks.hpp>

#include "config.h"

#include "logging/Logger.h" // Deprecated
#include "common/log.h"
#include "net/Manager.h"
#include "timer/Manager.h"
#include "config/Manager.h"
#include "can/Protocol.h"
#include "broker/Manager.h"
#include "control/Manager.h"
#include "can/Network.h"
#include "can/Monitor.h"
#include "can/CanDaemon.h"
#include "vm/Manager.h"
#include "storage/Manager.h"
#include "net/GetFile.h"
#include "net/Url.h"

#include "vm/plugin/System.h"
#include "vm/plugin/Timer.h"
#include "vm/plugin/Module.h"
#include "vm/plugin/Node.h"
#include "vm/plugin/Console.h"
#include "vm/plugin/Storage.h"
#include "vm/plugin/Socket.h"

#ifdef USE_PLUGIN_XORG
#include "vm/plugin/Xorg.h"
#endif // USE_PLUGIN_XORG

#ifdef USE_PLUGIN_MYSQL
#include "vm/plugin/MySql.h"
#endif // USE_PLUGIN_MYSQL

using namespace atom;

logging::Logger LOG("Main"); // Deprecated
static const std::string log_module_ = "main";
std::vector<broker::Subscriber::Pointer> subscribers;
boost::condition on_message_condition;
boost::mutex guard_mutex;
net::GetFile::Pointer downloader;

void ContinueInitialization(std::string protocol_filename);
void Handler(int status);
void CleanUp();




void HandleProtocolFileDownload(bool success, net::Url url, std::string temporary_filepath)
{
  if (success)
  {
    log::Info(log_module_, "Successfully downloaded protocol file!");
    ContinueInitialization(temporary_filepath);
  }
  else
  {
    ContinueInitialization(url.GetRaw());
  }
}

int main(int argc, char **argv)
{
  /* Set core dump parameters */
  rlimit core_limit = { RLIM_INFINITY, RLIM_INFINITY };
  setrlimit( RLIMIT_CORE, &core_limit ); // enable core dumps
  
  
  /* Print startup information */
  log::Info(log_module_, "Atom daemon, version %s starting...", VERSION);
  log::Info(log_module_, "Released under %s.", LICENSE);
  log::Info(log_module_, "Written by Mattias Runge 2010-2012.");

  
  /* Register signal handlers */
  signal(SIGTERM, Handler);
  signal(SIGINT,  Handler);
  signal(SIGQUIT, Handler);
  signal(SIGABRT, Handler);
  signal(SIGPIPE, Handler);

  
  /* Check configuration */
  config::Manager::Create();

  if (!config::Manager::Instance()->Set(argc, argv))
  {
    CleanUp();
    return EXIT_SUCCESS;
  }

  
  /* Set up logging */
  if (config::Manager::Instance()->Exist("LogFile"))
  {
    log::OpenFile(config::Manager::Instance()->GetAsString("LogFile"));
  }

  if (config::Manager::Instance()->Exist("LogLevelMask"))
  {
    log::SetLevelByString(config::Manager::Instance()->GetAsString("LogLevelMask"));
  }

  
  /* Enter daemon mode if requested */
  if (config::Manager::Instance()->Exist("daemon"))
  {
    LOG.Info("Entering daemon mode...");

    if (daemon(0, 0) == -1)
    {
      log::Error(log_module_, "Could not enter daemon mode. Exiting...");
      CleanUp();
      return EXIT_FAILURE;
    }

    log::Info(log_module_, "Deamon mode entered successfully!");
  }

  
  /* Create singletons */
  storage::Manager::Create();
  timer::Manager::Create();
  broker::Manager::Create();
  net::Manager::Create();
  can::Protocol::Create();
  control::Manager::Create();
  vm::Manager::Create();

  

  std::string protocol_filename = config::Manager::Instance()->GetAsString("ProtocolFile");
  
  try
  {
    net::Url url(protocol_filename);
    
    downloader = net::GetFile::Pointer(new net::GetFile());
    
    log::Info(log_module_, "Will try to download protocol file from \"%s\"...", protocol_filename.data());
    
    downloader->Start(url, HandleProtocolFileDownload);
  }
  catch (std::exception& exception)
  {
    log::Debug(log_module_, "Could not parse ProtocolFile as a URL, \"%s\".", protocol_filename.data());
    ContinueInitialization(protocol_filename);
  }

  
  /* Lock main thread and wait for exit */
  boost::mutex::scoped_lock guard(guard_mutex);

  on_message_condition.wait(guard);

  
  /* Cleanup */
  log::Info(log_module_, "Cleaning up...");

  CleanUp();

  log::Info(log_module_, "Thank you for using Atom. Goodbye!");

  return EXIT_SUCCESS;
}

void ContinueInitialization(std::string protocol_filename)
{  
  /* Load protocol file */
  if (!can::Protocol::Instance()->Load(protocol_filename))
  {
    log::Error(log_module_, "Failed to load %s!", protocol_filename.data());

    on_message_condition.notify_all();
  }
  
  
  /* Set up storage path */
  storage::Manager::Instance()->SetRootPath(config::Manager::Instance()->GetAsString("StoragePath"));

  
  /* Start monitor server */
  if (config::Manager::Instance()->Exist("MonitorPort"))
  {
    subscribers.push_back(can::Monitor::Pointer(new can::Monitor(config::Manager::Instance()->GetAsInt("MonitorPort"))));
  }

  
  /* Start CanDaemon server */
  if (config::Manager::Instance()->Exist("DaemonPort"))
  {
    subscribers.push_back(can::CanDaemon::Pointer(new can::CanDaemon(config::Manager::Instance()->GetAsInt("DaemonPort"))));
  }

  
  /* Subscribe Can control manager to data */
  subscribers.push_back(control::Manager::Instance());

  
  /* Start VM plugins */
  vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::System(vm::Manager::Instance()->GetIoService())));
  vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::Console(vm::Manager::Instance()->GetIoService(), config::Manager::Instance()->GetAsInt("CommandPort"))));
  vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::Storage(vm::Manager::Instance()->GetIoService())));
  vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::Timer(vm::Manager::Instance()->GetIoService())));
  vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::Module(vm::Manager::Instance()->GetIoService())));
  vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::Node(vm::Manager::Instance()->GetIoService())));
  vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::Socket(vm::Manager::Instance()->GetIoService())));

#ifdef USE_PLUGIN_XORG
  vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::Xorg(vm::Manager::Instance()->GetIoService())));
#endif // USE_PLUGIN_XORG
  
#ifdef USE_PLUGIN_MYSQL
  vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::MySql(vm::Manager::Instance()->GetIoService())));
#endif // USE_PLUGIN_MYSQL

  
  /* Start VM */
  vm::Manager::Instance()->Start(config::Manager::Instance()->GetAsString("ScriptPath"), config::Manager::Instance()->GetAsString("UserScriptPath"));

  
  /* Connect to Can network */
  if (config::Manager::Instance()->Exist("CanNet"))
  {
    common::StringList cannetworks = config::Manager::Instance()->GetAsStringVector("CanNet");

    for (uint n = 0; n < cannetworks.size(); n++)
    {
      subscribers.push_back(can::Network::Pointer(new can::Network(cannetworks[n])));
    }
  }
}

void CleanUp()
{
  downloader.reset();
  
  if (net::Manager::Instance().use_count() > 0)
  {
    net::Manager::Instance()->Stop();
  }

  subscribers.clear();
  config::Manager::Delete();
  timer::Manager::Delete();
  control::Manager::Delete();
  can::Protocol::Delete();
  broker::Manager::Delete();
  vm::Manager::Delete();
  net::Manager::Delete();
  storage::Manager::Delete();
  
  log::CloseFile();
}

void Handler(int status)
{
  std::string signal_name = "Unknown";

  switch (status)
  {
    case SIGTERM:
    {
      signal_name = "Terminate";
      break;
    }

    case SIGINT:
    {
      signal_name = "Interrupt";
      break;
    }

    case SIGQUIT:
    {
      signal_name = "Quit";
      break;
    }

    case SIGABRT:
    {
      signal_name = "Abort";
      break;
    }

    case SIGIO:
    {
      signal_name = "I/O";
      break;
    }

    case SIGPIPE:
    {
      signal_name = "Pipe";
      break;
    }
  }

  log::Info(log_module_, "Received signal %s (%d).", signal_name.c_str(), status);

  if (status != SIGPIPE)
  {
    on_message_condition.notify_all();
  }
}
