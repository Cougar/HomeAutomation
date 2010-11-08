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
namespace broker {
   
Subscriber::Subscriber(bool receive_from_self) : io_service_work_(io_service_)
{
    this->receive_from_self_ = receive_from_self;
    
    boost::thread thread(boost::bind(&boost::asio::io_service::run, &this->io_service_));
    this->thread_ = thread.move();
    
    this->tracker_ = TrackerPointer(new char);
    
    Manager::Instance()->ConnectSlots(Manager::SignalOnMessage::slot_type(&Subscriber::SlotOnMessage, this, _1).track(this->tracker_));
}

Subscriber::~Subscriber()
{
    this->tracker_.reset();
    
    this->thread_.interrupt();
    this->thread_.join();
}

void Subscriber::SlotOnMessage(Message::Pointer message)
{
    if (this->receive_from_self_ || !message->TestIfOrigin(this))
    {
        this->io_service_.post(boost::bind(&Subscriber::SlotOnMessageHandler, this, message));
    }
}
    
}; // namespace broker
}; // namespace atom
