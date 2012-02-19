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

#ifndef CAN_CAN_DAEMON_H
#define CAN_CAN_DAEMON_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "logging/Logger.h"
#include "broker/Subscriber.h"
#include "net/Subscriber.h"
#include "net/types.h"
#include "common/Byteset.h"

namespace atom {
namespace can {

class CanDaemon : public broker::Subscriber, public net::Subscriber
{
public:
    typedef boost::shared_ptr<CanDaemon> Pointer;
    
    CanDaemon(unsigned int port);
    virtual ~CanDaemon();
    
private:
    net::ServerId server_id_;
    
    void SlotOnMessageHandler(broker::Message::Pointer message);
    void SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state);
    void SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, common::Byteset data);
    
    logging::Logger LOG;
};

}; // namespace can
}; // namespace atom

#endif // CAN_CAN_DAEMON_H
