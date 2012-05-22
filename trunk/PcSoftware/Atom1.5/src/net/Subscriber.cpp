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

#include "Subscriber.h"

#include <boost/bind.hpp>

#include "Manager.h"
#include "common/log.h"

namespace atom {
namespace net {
  
static const std::string log_module_ = "net::subscriber"; 

Subscriber::Subscriber()
{
  LOG_DEBUG_ENTER;
  
  Manager::Instance()->ConnectSlots(Manager::SignalOnNewState::slot_type(&Subscriber::SlotOnNewState, this, _1, _2).track(this->tracker_),
                                    Manager::SignalOnNewClient::slot_type(&Subscriber::SlotOnNewClient, this, _1, _2).track(this->tracker_),
                                    Manager::SignalOnNewData::slot_type(&Subscriber::SlotOnNewData, this, _1, _2).track(this->tracker_));
  
  LOG_DEBUG_EXIT;
}

Subscriber::~Subscriber()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
}

void Subscriber::SlotOnNewData(SocketId id, common::Byteset data)
{
  LOG_DEBUG_ENTER;
  
  this->io_service_.post(boost::bind(&Subscriber::SlotOnNewDataHandler, this, id, data));
  
  LOG_DEBUG_EXIT;
}

void Subscriber::SlotOnNewClient(SocketId id, SocketId server_id)
{
  LOG_DEBUG_ENTER;
  
  this->io_service_.post(boost::bind(&Subscriber::SlotOnNewClientHandler, this, id, server_id));
  
  LOG_DEBUG_EXIT;
}


void Subscriber::SlotOnNewState(SocketId id, ClientState client_state)
{
  LOG_DEBUG_ENTER;
  
  this->io_service_.post(boost::bind(&Subscriber::SlotOnNewStateHandler, this, id, client_state));
  
  LOG_DEBUG_EXIT;
}


}; // namespace net
}; // namespace atom
