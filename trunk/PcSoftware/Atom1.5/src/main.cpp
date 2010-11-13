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

#include <boost/program_options.hpp>

#include "net/Manager.h"
#include "net/types.h"

#include "timer/Manager.h"
#include "timer/types.h"

#include "config/Manager.h"

#include "logging/Logger.h"

#include "can/Network.h"
#include "can/Monitor.h"
#include "can/Manager.h"
#include "can/Protocol.h"

using namespace atom;
 
logging::Logger LOG("Main");

void CleanUp();

int main(int argc, char **argv)
{
    std::vector<broker::Subscriber::Pointer> subscribers;
 
    if (!config::Manager::Instance()->Set(argc, argv))
    {
        CleanUp();
        return EXIT_SUCCESS;
    }
    
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
    
    sleep(100000000);
    
    subscribers.clear();
    
    CleanUp();
    
    return EXIT_SUCCESS;
}

void CleanUp()
{
    config::Manager::Delete();
    net::Manager::Delete();
    can::Manager::Delete();
    timer::Manager::Delete();
}