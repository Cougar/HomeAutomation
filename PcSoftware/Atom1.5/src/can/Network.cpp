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

#include "Network.h"

#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "net/Manager.h"

namespace atom {
namespace can {

Network::Network(std::string address): Subscriber(false), LOG("can::Network")
{
    this->address_ = address;
    this->client_id_ = 0;
       
    net::Manager::Instance()->ConnectSlots(net::Client::SignalOnNewState::slot_type(&Network::SlotOnNewState, this, _1, _2, _3).track(this->tracker_),
                                           net::Client::SignalOnNewData::slot_type(&Network::SlotOnNewData, this, _1, _2, _3).track(this->tracker_));
    
    // Examples of address
    // udp:192.168.1.250:1100
    // serial:/dev/ttyUSB0:38400
    
    std::vector<std::string> parts;
    boost::algorithm::split(parts, address, boost::is_any_of(":"), boost::algorithm::token_compress_off);
    
    if (parts.size() < 3)
    {
        LOG.Error("Malformed address string: " + address);
        return;
    }
    
    boost::algorithm::to_lower(parts[0]);
    
    if (parts[0] == "udp")
    {
        this->protocol_ = net::PROTOCOL_UDP;
    }
    else if (parts[0] == "serial")
    {
        this->protocol_ = net::PROTOCOL_SERIAL;
    }
    else
    {
        LOG.Error("Unknown protocol, only support udp and serial, got " + parts[0]);
        return;
    }
    
    this->address_ = parts[1];
    this->port_or_baud_ = boost::lexical_cast<unsigned int>(parts[2]);
    
    try
    {
        this->client_id_ = net::Manager::Instance()->Connect(this->protocol_, this->address_, this->port_or_baud_);
        LOG.Info("Connected to CAN network at " + address);
        
        // TODO: Send ping
    }
    catch (std::exception e)
    {
        LOG.Error(e.what());
    }
}

Network::~Network()
{
    net::Manager::Instance()->Disconnect(this->client_id_);
}

void Network::SlotOnNewData(net::ClientId client_id, net::ServerId server_id, net::Buffer data)
{
    if (client_id == this->client_id_)
    {
        this->io_service_.post(boost::bind(&Network::SlotOnNewDataHandler, this, client_id, server_id, data));
    }
}

void Network::SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    if (client_id == this->client_id_)
    {
        this->io_service_.post(boost::bind(&Network::SlotOnNewStateHandler, this, client_id, server_id, client_state));
    }
}

void Network::SlotOnMessageHandler(broker::Message::Pointer message)
{
    // TODO: Send to network
}

void Network::SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, net::Buffer data)
{
    // TODO: Send to broker
}

void Network::SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    if (client_state == net::CLIENT_STATE_DISCONNECTED)
    {
        LOG.Warning("Got disconnected from CAN network, will try to reconnect...");
     
        this->client_id_ = 0;
        
        try
        {
            this->client_id_ = net::Manager::Instance()->Connect(this->protocol_, this->address_, this->port_or_baud_);
            LOG.Info("Connected to CAN network again.");
        }
        catch (std::exception e)
        {
            LOG.Error(e.what());
        }
    }
    else
    {
        LOG.Debug("Got state: " + boost::lexical_cast<std::string>((int)client_state));
    }
}
    
}; // namespace can
}; // namespace atom
