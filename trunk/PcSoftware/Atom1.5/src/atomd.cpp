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

#include "logging/Logger.h"
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

logging::Logger LOG("Main");
std::vector<broker::Subscriber::Pointer> subscribers;
boost::condition on_message_condition;
boost::mutex guard_mutex;

void Handler(int status);
void CleanUp();

int main(int argc, char **argv)
{
  rlimit core_limit = { RLIM_INFINITY, RLIM_INFINITY };
  setrlimit( RLIMIT_CORE, &core_limit ); // enable core dumps
      
  
  LOG.Info("\033[29;1mAtom Daemon, version " + std::string(VERSION) + " starting...\033[0m");
  LOG.Info("\033[29;1mReleased under " + std::string(LICENSE) + ".\033[0m");
  LOG.Info("Written by Mattias Runge 2010-2012.");

  signal(SIGTERM, Handler);
  signal(SIGINT, Handler);
  signal(SIGQUIT, Handler);
  signal(SIGABRT, Handler);
  signal(SIGPIPE, Handler);

  config::Manager::Create();

  if (!config::Manager::Instance()->Set(argc, argv))
  {
    CleanUp();
    return EXIT_SUCCESS;
  }

  if (config::Manager::Instance()->Exist("LogFile"))
  {
    logging::Logger::OpenFile(config::Manager::Instance()->GetAsString("LogFile"));
  }

  if (config::Manager::Instance()->Exist("LogLevel"))
  {
    logging::Logger::SetLevel((logging::Logger::Level) config::Manager::Instance()->GetAsInt("LogLevel"));
  }

  if (config::Manager::Instance()->Exist("daemon"))
  {
    LOG.Info("Entering daemon mode...");

    if (daemon(0, 0) == -1)
    {
      LOG.Error("Could not enter daemon mode. Exiting...");
      CleanUp();
      return EXIT_FAILURE;
    }

    LOG.Info("Deamon mode entered successfully!");
  }

  storage::Manager::Create();
  timer::Manager::Create();
  broker::Manager::Create();
  net::Manager::Create();
  can::Protocol::Create();
  control::Manager::Create();
  vm::Manager::Create();

  storage::Manager::Instance()->SetRootPath(config::Manager::Instance()->GetAsString("StoragePath"));

  can::Protocol::Instance()->Load(config::Manager::Instance()->GetAsString("ProtocolFile"));

  if (config::Manager::Instance()->Exist("MonitorPort"))
  {
    subscribers.push_back(can::Monitor::Pointer(new can::Monitor(config::Manager::Instance()->GetAsInt("MonitorPort"))));
  }

  if (config::Manager::Instance()->Exist("DaemonPort"))
  {
    subscribers.push_back(can::CanDaemon::Pointer(new can::CanDaemon(config::Manager::Instance()->GetAsInt("DaemonPort"))));
  }

  subscribers.push_back(control::Manager::Instance());

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

  vm::Manager::Instance()->Start(config::Manager::Instance()->GetAsString("ScriptPath"), config::Manager::Instance()->GetAsString("UserScriptPath"));

  if (config::Manager::Instance()->Exist("CanNet"))
  {
    common::StringList cannetworks = config::Manager::Instance()->GetAsStringVector("CanNet");

    for (uint n = 0; n < cannetworks.size(); n++)
    {
      subscribers.push_back(can::Network::Pointer(new can::Network(cannetworks[n])));
    }
  }

  boost::mutex::scoped_lock guard(guard_mutex);

  on_message_condition.wait(guard);

  LOG.Info("Cleaning up...");

  CleanUp();

  LOG.Info("Thank you for using Atom. Goodbye!");

  return EXIT_SUCCESS;
}

void CleanUp()
{
  net::Manager::Instance()->Stop();

  subscribers.clear();
  config::Manager::Delete();
  timer::Manager::Delete();
  control::Manager::Delete();
  can::Protocol::Delete();
  broker::Manager::Delete();
  vm::Manager::Delete();
  net::Manager::Delete();
  storage::Manager::Delete();
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

  LOG.Debug("Received signal " + signal_name + "(" + boost::lexical_cast<std::string>(status) + ").");

  if (status != SIGPIPE)
  {
    on_message_condition.notify_all();
  }
}
