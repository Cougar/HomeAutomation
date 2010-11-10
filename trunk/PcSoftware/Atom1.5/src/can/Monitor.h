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

#ifndef CAN_MONITOR_H
#define CAN_MONITOR_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "logging/Logger.h"
#include "broker/Subscriber.h"
#include "net/types.h"
#include "type/Byteset.h"

namespace atom {
namespace can {

class Monitor : public broker::Subscriber
{
public:
    typedef boost::shared_ptr<Monitor> Pointer;
    
    Monitor(unsigned int port);
    virtual ~Monitor();
    
private:
    net::ServerId server_id_;
    
    void SlotOnMessageHandler(broker::Message::Pointer message);
    
    void SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state);
    void SlotOnNewData(net::ClientId client_id, net::ServerId server_id, type::Byteset data);
    
    void SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state);
    void SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, type::Byteset data);
    
    logging::Logger LOG;
};

}; // namespace can
}; // namespace atom

#endif // CAN_MONITOR_H
