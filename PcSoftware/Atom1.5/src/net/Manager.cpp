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

#include "common/log.h"

static const std::string log_module_ = "net::manager";

namespace atom {
namespace net {

Manager::Pointer Manager::instance_;


Manager::Manager()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
}

Manager::~Manager()
{
  LOG_DEBUG_ENTER;
  
  this->Stop();
  
  this->clients_.clear();
  
  LOG_DEBUG_EXIT;
}

Manager::Pointer Manager::Instance()
{
  LOG_DEBUG_ENTER;
  
  return Manager::instance_;
  
  LOG_DEBUG_EXIT;
}

void Manager::Create()
{
  LOG_DEBUG_ENTER;
  
  Manager::instance_ = Manager::Pointer(new Manager());
  
  LOG_DEBUG_EXIT;
}

void Manager::Delete()
{
  LOG_DEBUG_ENTER;
  
  Manager::instance_.reset();
  
  LOG_DEBUG_EXIT;
}

void Manager::ConnectSlots(const SignalOnNewState::slot_type& slot_on_new_state, const SignalOnNewData::slot_type& slot_on_new_data)
{
  LOG_DEBUG_ENTER;
  
  this->signal_on_new_state_.connect(slot_on_new_state);
  this->signal_on_new_data_.connect(slot_on_new_data);
  
  LOG_DEBUG_EXIT;
}

void Manager::SlotOnNewState(SocketId client_id, SocketId server_id, ClientState client_state)
{
  LOG_DEBUG_ENTER;
  
  log::Debug(log_module_, "SlotOnNewState, client_id %d, server_id %d, client_state %d", client_id, server_id, client_state);
  
  if (client_state == CLIENT_STATE_DISCONNECTED)
  {
    this->clients_.erase(client_id);
  }
  
  this->signal_on_new_state_(client_id, server_id, client_state);
  
  LOG_DEBUG_EXIT;
}

void Manager::SlotOnNewClient(SocketId server_id, TcpSocketPointer socket)
{
  LOG_DEBUG_ENTER;
  
  TcpClient::Pointer client = TcpClient::Pointer(new TcpClient(this->io_service_, socket, this->GetFreeSocketId(), server_id));
  
  client->ConnectSlots(Client::SignalOnNewState::slot_type(&Manager::SlotOnNewState, this, _1, _2, _3).track(Manager::instance_),
                       Client::SignalOnNewData::slot_type(&Manager::SlotOnNewData, this, _1, _2, _3).track(Manager::instance_));
  
  this->clients_[client->GetId()] = client;

  log::Debug(log_module_, "Client connected on server id %d with client id %d!", server_id, client->GetId());
  
  this->signal_on_new_state_(client->GetId(), server_id, CLIENT_STATE_CONNECTED);
  
  LOG_DEBUG_EXIT;
}

void Manager::SlotOnNewData(SocketId client_id, SocketId server_id, common::Byteset data)
{
  LOG_DEBUG_ENTER;
  
  this->signal_on_new_data_(client_id, server_id, data);
  
  LOG_DEBUG_EXIT;
}

SocketId Manager::GetFreeSocketId()
{
  LOG_DEBUG_ENTER;
  
  SocketId id = 1;
  
  while (this->clients_.find(id) != this->clients_.end() || this->servers_.find(id) != this->servers_.end())
  {
    id++;
  }
  
  LOG_DEBUG_EXIT;
  
  return id;
}

SocketId Manager::StartServer(Protocol protocol, unsigned int port)
{
  LOG_DEBUG_ENTER;
  
  if (protocol != PROTOCOL_TCP)
  {
    throw std::runtime_error("Can not start server for the Serial or UDP protocol!");
  }
  
  TcpServer::Pointer server = TcpServer::Pointer(new TcpServer(this->io_service_, port, this->GetFreeSocketId()));
  
  server->ConnectSlots(TcpServer::SignalOnNewClient::slot_type(&Manager::SlotOnNewClient, this, _1, _2).track(Manager::instance_));

  server->Accept();
  
  this->servers_[server->GetId()] = server;
  
  log::Debug(log_module_, "Started server with id %d!", server->GetId());
  
  LOG_DEBUG_EXIT;
  
  return server->GetId();
}

SocketId Manager::Connect(Protocol protocol, std::string address, unsigned int port_or_baud)
{
  LOG_DEBUG_ENTER;
  
  Client::Pointer client;
  
  switch (protocol)
  {
    case PROTOCOL_TCP:
    {
      client = Client::Pointer(new TcpClient(this->io_service_, this->GetFreeSocketId(), 0));
      break;
    }
    case PROTOCOL_UDP:
    {
      client = Client::Pointer(new UdpClient(this->io_service_, this->GetFreeSocketId(), 0));
      break;
    }
    case PROTOCOL_SERIAL:
    {
      client = Client::Pointer(new SerialClient(this->io_service_, this->GetFreeSocketId(), 0));
      break;
    }
    default:
    {
      throw std::runtime_error("Invalid protocol specified!");
    }
  }
  
  client->ConnectSlots(Client::SignalOnNewState::slot_type(&Manager::SlotOnNewState, this, _1, _2, _3).track(Manager::instance_),
                       Client::SignalOnNewData::slot_type(&Manager::SlotOnNewData, this, _1, _2, _3).track(Manager::instance_));
  
  client->Connect(address, port_or_baud);
  
  this->clients_[client->GetId()] = client;
  
  LOG_DEBUG_EXIT;
  
  return client->GetId();
}

void Manager::SendToAll(SocketId server_id, common::Byteset data)
{
  LOG_DEBUG_ENTER;
  
  this->io_service_.post(boost::bind(&Manager::SendToAllHandler, this, server_id, data));
  
  LOG_DEBUG_EXIT;
}

void Manager::SendTo(SocketId client_id, common::Byteset data)
{
  LOG_DEBUG_ENTER;
  
  log::Debug(log_module_, "SendTo, client_id %d", client_id);
  
  this->io_service_.post(boost::bind(&Manager::SendToHandler, this, client_id, data));
  
  LOG_DEBUG_EXIT;
}

void Manager::StopServer(SocketId server_id)
{
  LOG_DEBUG_ENTER;
  
  this->io_service_.post(boost::bind(&Manager::StopServerHandler, this, server_id));
  
  LOG_DEBUG_EXIT;
}

void Manager::Disconnect(SocketId client_id)
{
  LOG_DEBUG_ENTER;
  
  log::Debug(log_module_, "Disconnect, client_id %d", client_id);
  
  this->io_service_.post(boost::bind(&Manager::DisconnectHandler, this, client_id));
  
  LOG_DEBUG_EXIT;
}

void Manager::SendToAllHandler(SocketId server_id, common::Byteset data)
{
  LOG_DEBUG_ENTER;
  
  for (ClientList::iterator it = this->clients_.begin(); it != this->clients_.end(); it++)
  {
    if (it->second->GetServerId() == server_id)
    {
      it->second->Send(data);
    }
  }
  
  LOG_DEBUG_EXIT;
}

void Manager::SendToHandler(SocketId client_id, common::Byteset data)
{
  LOG_DEBUG_ENTER;
  
  log::Debug(log_module_, "SendToHandler, client_id %d, data %s", client_id, data.ToCharString().c_str());
  
  ClientList::iterator it = this->clients_.find(client_id);

  if (it != this->clients_.end())
  {
    it->second->Send(data);
  }
  
  LOG_DEBUG_EXIT;
}

void Manager::StopServerHandler(SocketId server_id)
{
  LOG_DEBUG_ENTER;
  
  this->clients_.erase(server_id);
  
  LOG_DEBUG_EXIT;
}

void Manager::DisconnectHandler(SocketId client_id)
{
  LOG_DEBUG_ENTER;
  
  log::Debug(log_module_, "DisconnectHandler, client_id %d", client_id);
  
  ClientList::iterator it = this->clients_.find(client_id);
  
  if (it != this->clients_.end())
  {
    it->second->Disconnect();
  }
  
  LOG_DEBUG_EXIT;
}


}; // namespace net
}; // namespace atom
