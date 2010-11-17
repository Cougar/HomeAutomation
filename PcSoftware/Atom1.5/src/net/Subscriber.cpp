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

namespace atom {
namespace net {

Subscriber::Subscriber()
{
    Manager::Instance()->ConnectSlots(Manager::SignalOnNewState::slot_type(&Subscriber::SlotOnNewState, this, _1, _2, _3).track(this->tracker_),
                                      Manager::SignalOnNewData::slot_type(&Subscriber::SlotOnNewData, this, _1, _2, _3).track(this->tracker_));
}

Subscriber::~Subscriber()
{
}

void Subscriber::SlotOnNewData(ClientId client_id, ServerId server_id, type::Byteset data)
{
    type::Byteset temp_buffer(data);
    this->io_service_.post(boost::bind(&Subscriber::SlotOnNewDataHandler, this, client_id, server_id, temp_buffer));
}

void Subscriber::SlotOnNewState(ClientId client_id, ServerId server_id, ClientState client_state)
{
    this->io_service_.post(boost::bind(&Subscriber::SlotOnNewStateHandler, this, client_id, server_id, client_state));
}

}; // namespace net
}; // namespace atom
