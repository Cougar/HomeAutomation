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

#include "Monitor.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "net/Manager.h"

#include "Message.h"

namespace atom {
namespace can {
    
Monitor::Monitor(unsigned int port): Subscriber(port), LOG("can::Monitor")
{
    net::Manager::Instance()->ConnectSlots(net::Client::SignalOnNewState::slot_type(&Monitor::SlotOnNewState, this, _1, _2, _3).track(this->tracker_),
                                           net::Client::SignalOnNewData::slot_type(&Monitor::SlotOnNewData, this, _1, _2, _3).track(this->tracker_));
    
    try
    {
        this->server_id_ = net::Manager::Instance()->StartServer(net::PROTOCOL_TCP, port);
        LOG.Info("Started TCP server on port " + boost::lexical_cast<std::string>(port));
        LOG.Debug("Server id is " + boost::lexical_cast<std::string>(this->server_id_));
    }
    catch (std::exception e)
    {
        LOG.Error(e.what());
    }
}

Monitor::~Monitor()
{
    net::Manager::Instance()->StopServer(this->server_id_);
}


void Monitor::SlotOnNewData(net::ClientId client_id, net::ServerId server_id, type::Byteset data)
{
    if (server_id == this->server_id_)
    {
        type::Byteset temp_buffer = data;
        this->io_service_.post(boost::bind(&Monitor::SlotOnNewDataHandler, this, client_id, server_id, temp_buffer));
    }
}

void Monitor::SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    if (server_id == this->server_id_)
    {
        this->io_service_.post(boost::bind(&Monitor::SlotOnNewStateHandler, this, client_id, server_id, client_state));
    }
}

void Monitor::SlotOnMessageHandler(broker::Message::Pointer message)
{
    if (message->GetType() == broker::Message::CAN_MESSAGE)
    {
        Message* payload = static_cast<Message*>(message->GetPayload().get());
        
        std::string line = "";
        
        if (payload->GetClassName() == "nmt")
        {
            line += "NMT";
        }
        else
        {
            if (payload->GetDirectionName() == "To_Owner")
            {
                line += "RX";
            }
            else if (payload->GetDirectionName() == "From_Owner")
            {
                line += "TX";
            }
            else
            {
                line += "??";
            }
            
            line += " " + payload->GetClassName();
            line += "_" + payload->GetModuleName();
            line += ":" + boost::lexical_cast<std::string>(payload->GetId());
        }
        
        line += " CMD=" + payload->GetCommandName() + " ";
        
        Message::VariableList variables = payload->GetVariables();
        
        for (Message::VariableList::iterator it = variables.begin(); it != variables.end(); it++)
        {
            line += " " + it->first + "=" + it->second;
        }
        
        line += "\n";
        
        net::Manager::Instance()->SendToAll(this->server_id_, type::Byteset(line));
    }
}

void Monitor::SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, type::Byteset data)
{
    std::string str((char*)data.Get());
    
    boost::algorithm::trim_if(str, boost::is_any_of("\n\r"));
    
    LOG.Debug("Received: \"" + str + "\" from client " + boost::lexical_cast<std::string>(client_id) + " on server " + boost::lexical_cast<std::string>(server_id));
    
    if (str == "q" || str == "quit")
    {
        LOG.Info("Client " + boost::lexical_cast<std::string>(client_id) + " has requested to be disconnected.");
        net::Manager::Instance()->Disconnect(client_id);
    }
}

void Monitor::SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    if (client_state == net::CLIENT_STATE_DISCONNECTED)
    {
        LOG.Info("Client " + boost::lexical_cast<std::string>(client_id) + " has disconnected.");
    }
    else if (client_state == net::CLIENT_STATE_CONNECTED)
    {
        LOG.Info("Client " + boost::lexical_cast<std::string>(client_id) + " has connected.");
        net::Manager::Instance()->SendTo(client_id, type::Byteset("Welcome to Atom CAN monitoring\n"));
    }
    else
    {
        LOG.Debug("Got state: " + boost::lexical_cast<std::string>((int)client_state));
    }
}
    
}; // namespace can
}; // namespace atom
