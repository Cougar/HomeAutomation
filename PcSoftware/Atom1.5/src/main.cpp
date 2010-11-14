/*
 * 
 *  Copyright (C) 2010  Mattias Runge
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
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

#include <boost/program_options.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/locks.hpp>

#include "logging/Logger.h"
#include "net/Manager.h"
#include "timer/Manager.h"
#include "config/Manager.h"
#include "can/Protocol.h"
#include "broker/Manager.h"
#include "can/Manager.h"
#include "can/Network.h"
#include "can/Monitor.h"
#include "vm/Manager.h"

#include "vm/plugin/System.h"
#include "vm/plugin/Timer.h"
#include "vm/plugin/Module.h"

using namespace atom;
 
logging::Logger LOG("Main");
std::vector<broker::Subscriber::Pointer> subscribers;
boost::condition on_message_condition;
boost::mutex guard_mutex;

void Handler(int status);
void CleanUp();

int main(int argc, char **argv)
{
    LOG.Info("Atom version 1.5.0 starting...");
    LOG.Info("Written by Mattias Runge 2010.");
    LOG.Info("Released under GPL version 2.");
    
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
        logging::Logger::SetLevel((logging::Logger::Level)config::Manager::Instance()->GetAsInt("LogLevel"));
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

    timer::Manager::Create();
    broker::Manager::Create();
    net::Manager::Create();
    can::Protocol::Create();
    can::Manager::Create();
    vm::Manager::Create();
    
    if (config::Manager::Instance()->Exist("ProtocolFile"))
    {
        can::Protocol::Instance()->Load(config::Manager::Instance()->GetAsString("ProtocolFile"));
    }
    
    if (config::Manager::Instance()->Exist("MonitorPort"))
    {
        subscribers.push_back(can::Monitor::Pointer(new can::Monitor(config::Manager::Instance()->GetAsInt("MonitorPort"))));  
    }
    
    subscribers.push_back(can::Manager::Instance());  
    
    if (config::Manager::Instance()->Exist("CanNet"))
    {
        type::StringList cannetworks = config::Manager::Instance()->GetAsStringVector("CanNet");
        
        for (int n = 0; n < cannetworks.size(); n++)
        {
            subscribers.push_back(can::Network::Pointer(new can::Network(cannetworks[n])));            
        }
    }

    vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::System()));
    vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::Timer()));
    vm::Manager::Instance()->AddPlugin(vm::Plugin::Pointer(new vm::plugin::Module()));

    if (config::Manager::Instance()->Exist("ScriptPath"))
    {
        vm::Manager::Instance()->Start(config::Manager::Instance()->GetAsString("ScriptPath"));
    }
    
    LOG.Info("Initialization complete.");
    
    boost::mutex::scoped_lock guard(guard_mutex);
    on_message_condition.wait(guard);
    
    LOG.Info("Cleaning up...");
    
    CleanUp();
    
    LOG.Info("Thank you for using Atom. Goodbye!");
    
    return EXIT_SUCCESS;
}

void CleanUp()
{
    subscribers.clear();
    config::Manager::Delete();
    timer::Manager::Delete();
    can::Manager::Delete();
    vm::Manager::Delete();
    can::Protocol::Delete();
    broker::Manager::Delete();
    net::Manager::Delete();
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
            signal_name = "Interupt";
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