/*
 * 
 *    Copyright (C) 2010  Mattias Runge
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
*/

#ifndef CAN_NETWORK_H
#define CAN_NETWORK_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "logging/Logger.h"
#include "broker/Subscriber.h"
#include "net/Subscriber.h"
#include "net/types.h"
#include "common/Byteset.h"
#include "timer/Manager.h"

namespace atom {
namespace can {

class Network : public broker::Subscriber, public net::Subscriber
{
public:
    typedef boost::shared_ptr<Network> Pointer;
    
    Network(std::string address);
    virtual ~Network();
    
private:
    net::SocketId client_id_;
    net::Protocol protocol_;
    std::string address_;
    unsigned int port_or_baud_;
    common::Byteset buffer_;    
    timer::TimerId timer_id_;
    
    void SlotOnMessageHandler(broker::Message::Pointer message);
    
    void SlotOnTimeout(timer::TimerId timer_id);
    
    void SlotOnNewStateHandler(net::SocketId id, net::ClientState client_state);
    void SlotOnNewClientHandler(net::SocketId id, net::SocketId server_id);
    void SlotOnNewDataHandler(net::SocketId id, common::Byteset data);
    void SlotOnTimeoutHandler(timer::TimerId timer_id);
    
    void ProcessBuffer();
    
    logging::Logger LOG;    
};

}; // namespace can
}; // namespace atom

#endif // CAN_NETWORK_H
