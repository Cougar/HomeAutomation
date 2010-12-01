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

#include "Manager.h"

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/cast.hpp>

namespace atom {
namespace net {

Manager::Pointer Manager::instance_;
    
Manager::Manager()
{

}

Manager::~Manager()
{
    this->clients_.clear();
}

Manager::Pointer Manager::Instance()
{
    return Manager::instance_;
}

void Manager::Create()
{
    Manager::instance_ = Manager::Pointer(new Manager());
}

void Manager::Delete()
{
    Manager::instance_.reset();
}

void Manager::ConnectSlots(const SignalOnNewState::slot_type& slot_on_new_state, const SignalOnNewData::slot_type& slot_on_new_data)
{
    this->signal_on_new_state_.connect(slot_on_new_state);
    this->signal_on_new_data_.connect(slot_on_new_data);
}

void Manager::SlotOnNewState(ClientId client_id, ServerId server_id, ClientState client_state)
{
    if (client_state == CLIENT_STATE_DISCONNECTED)
    {
        this->clients_.erase(client_id);
    }
    else if (client_state == CLIENT_STATE_ACCEPTED)
    {
        ClientList::iterator it = this->clients_.find(client_id);
        
        if (it != this->clients_.end())
        {
            TcpClient::Pointer client = TcpClient::Pointer(new TcpClient(this->io_service_, this->GetFreeClientId(), it->second->GetServerId()));
            
            client->ConnectSlots(Client::SignalOnNewState::slot_type(&Manager::SlotOnNewState, this, _1, _2, _3).track(Manager::instance_),
                                 Client::SignalOnNewData::slot_type(&Manager::SlotOnNewData, this, _1, _2, _3).track(Manager::instance_));
            
            client->Accept(boost::polymorphic_downcast<TcpClient*>(it->second.get())->ReleaseAcceptor());
            
            this->clients_[client->GetId()] = client;
            
            this->signal_on_new_state_(client_id, server_id, CLIENT_STATE_CONNECTED);
            return;
        }
        else
        {
            throw std::runtime_error("Accepted unknown client!");
        }
    }
    
    this->signal_on_new_state_(client_id, server_id, client_state);
}

void Manager::SlotOnNewData(ClientId client_id, ServerId server_id, type::Byteset data)
{
    this->signal_on_new_data_(client_id, server_id, data);
}

ServerId Manager::GetFreeServerId()
{
    ServerId server_id = 1;
    
    for (ClientList::iterator it = this->clients_.begin(); it != this->clients_.end(); it++)
    {
        if (it->second->GetServerId() != server_id)
        {
            return server_id;
        }
        
        server_id++;
    }
    
    return server_id;
}

ClientId Manager::GetFreeClientId()
{
    ClientId client_id = 1;
    
    while (this->clients_.find(client_id) != this->clients_.end())
    {
        client_id++;
    }
    
    return client_id;
}

ServerId Manager::StartServer(Protocol protocol, unsigned int port)
{
    if (protocol != PROTOCOL_TCP)
    {
        throw std::runtime_error("Can not start server for the Serial or UDP protocol!");
        return 0;
    }
    
    TcpClient::Pointer client = TcpClient::Pointer(new TcpClient(this->io_service_, this->GetFreeClientId(), this->GetFreeServerId()));
    
    client->ConnectSlots(Client::SignalOnNewState::slot_type(&Manager::SlotOnNewState, this, _1, _2, _3).track(Manager::instance_),
                         Client::SignalOnNewData::slot_type(&Manager::SlotOnNewData, this, _1, _2, _3).track(Manager::instance_));
    
    TcpClient::AcceptorPointer acceptor = TcpClient::AcceptorPointer(new boost::asio::ip::tcp::acceptor(this->io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)));    

    client->Accept(acceptor);
    
    this->clients_[client->GetId()] = client;
    
    return client->GetServerId();
}

ClientId Manager::Connect(Protocol protocol, std::string address, unsigned int port_or_baud)
{
    Client::Pointer client;
    
    switch (protocol)
    {
        case PROTOCOL_TCP:
        {
            client = Client::Pointer(new TcpClient(this->io_service_, this->GetFreeClientId(), 0));
            break;
        }
        case PROTOCOL_UDP:
        {
            client = Client::Pointer(new UdpClient(this->io_service_, this->GetFreeClientId(), 0));
            break;
        }
        case PROTOCOL_SERIAL:
        {
            client = Client::Pointer(new SerialClient(this->io_service_, this->GetFreeClientId(), 0));
            break;
        }
        default:
        {
            throw std::runtime_error("Invalid protocol specified!");
            return 0;
        }
    }
    
    client->ConnectSlots(Client::SignalOnNewState::slot_type(&Manager::SlotOnNewState, this, _1, _2, _3).track(Manager::instance_),
                         Client::SignalOnNewData::slot_type(&Manager::SlotOnNewData, this, _1, _2, _3).track(Manager::instance_));
    
    client->Connect(address, port_or_baud);
    
    this->clients_[client->GetId()] = client;
    
    return client->GetId();
}

void Manager::SendToAll(ServerId server_id, type::Byteset data)
{
    this->io_service_.post(boost::bind(&Manager::SendToAllHandler, this, server_id, data));
}

void Manager::SendTo(ClientId client_id, type::Byteset data)
{
    this->io_service_.post(boost::bind(&Manager::SendToHandler, this, client_id, data));
}

void Manager::StopServer(ServerId server_id)
{
    this->io_service_.post(boost::bind(&Manager::StopServerHandler, this, server_id));
}

void Manager::Disconnect(ClientId client_id)
{
    this->io_service_.post(boost::bind(&Manager::DisconnectHandler, this, client_id));
}

void Manager::SendToAllHandler(ServerId server_id, type::Byteset data)
{
    for (ClientList::iterator it = this->clients_.begin(); it != this->clients_.end(); it++)
    {
        if (it->second->GetServerId() == server_id)
        {
            it->second->Send(data);
        }
    }
}

void Manager::SendToHandler(ClientId client_id, type::Byteset data)
{
    ClientList::iterator it = this->clients_.find(client_id);

    if (it != this->clients_.end())
    {
        it->second->Send(data);
    }
}

void Manager::StopServerHandler(ServerId server_id)
{
    for (ClientList::iterator it = this->clients_.begin(); it != this->clients_.end(); it++)
    {
        if (it->second->GetServerId() == server_id)
        {
            it->second->Stop();
        }
    }
}

void Manager::DisconnectHandler(ClientId client_id)
{
    ClientList::iterator it = this->clients_.find(client_id);
    
    if (it != this->clients_.end())
    {
        it->second->Disconnect();
    }
}

}; // namespace net
}; // namespace atom
