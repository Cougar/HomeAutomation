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

#include "Client.h"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "common/log.h"

namespace atom {
namespace net {
  
static const std::string log_module_ = "net::client";

Client::Client(boost::asio::io_service& io_service, SocketId id, SocketId server_id) : buffer_(2048), io_service_(io_service)
{
  LOG_DEBUG_ENTER;
  
  this->server_id_ = server_id;
  this->id_ = id;
  
  LOG_DEBUG_EXIT;
}

Client::~Client()
{
  LOG_DEBUG_ENTER;
  
  this->Stop();
  
  LOG_DEBUG_EXIT;
}

void Client::ConnectSlots(const SignalOnNewState::slot_type& slot_on_new_state, const SignalOnNewData::slot_type& slot_on_new_data)
{
  LOG_DEBUG_ENTER;
  
  this->signal_on_new_state_.connect(slot_on_new_state);
  this->signal_on_new_data_.connect(slot_on_new_data);
  
  LOG_DEBUG_EXIT;
}

SocketId Client::GetId()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
  
  return this->id_;
}

SocketId Client::GetServerId()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
  
  return this->server_id_;
}

void Client::Read()
{
  LOG_DEBUG_ENTER;
  
  this->buffer_.Clear();
  
  LOG_DEBUG_EXIT;
}

void Client::ReadHandler(const boost::system::error_code& error, size_t size)
{
  LOG_DEBUG_ENTER;

  
  if (error)
  {
    log::Debug(log_module_, "error %d", error);
    
    if (boost::system::errc::operation_canceled != error)
    {
      this->Disconnect();
    }
  }
  else if (size == 0)
  {
    log::Debug(log_module_, "size 0");
    
    this->Disconnect();
  }
  else
  {
    this->buffer_.SetSize(size);
    
    this->signal_on_new_data_(this->id_, this->server_id_, this->buffer_);
    
    this->Read();
  }
  
  LOG_DEBUG_EXIT;
}

void Client::Stop()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
}

void Client::Disconnect()
{
  LOG_DEBUG_ENTER;
  
  if (this->id_ != 0)
  {
    SocketId id = this->id_;
      
    this->Stop();
        
    this->id_ = 0;
        
    this->signal_on_new_state_(id, this->server_id_, CLIENT_STATE_DISCONNECTED);
  }
  
  LOG_DEBUG_EXIT;
}
    
}; // namespace net
}; // namespace atom
