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

#ifndef BROKER_SUBSCRIBER_H
#define BROKER_SUBSCRIBER_H

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "Message.h"

namespace atom {
namespace broker {

class Subscriber : public Origin
{
public:
    typedef boost::shared_ptr<Subscriber> Pointer;
    
    Subscriber(bool receive_from_self);
    virtual ~Subscriber();
    
protected:
    typedef boost::shared_ptr<char> TrackerPointer;
    
    boost::asio::io_service io_service_;
    TrackerPointer tracker_;
    
    virtual void SlotOnMessageHandler(Message::Pointer message) = 0;
    
private:
    boost::thread thread_;
    boost::asio::io_service::work io_service_work_;
    
    bool receive_from_self_;
    
    void SlotOnMessage(Message::Pointer message);
};

}; // namespace broker
}; // namespace atom

#endif // BROKER_SUBSCRIBER_H
