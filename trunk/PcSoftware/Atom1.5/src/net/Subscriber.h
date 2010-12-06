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

#ifndef NET_SUBSCRIBER_H
#define NET_SUBSCRIBER_H

#include <boost/shared_ptr.hpp>

#include "common/IoService.h"
#include "common/Byteset.h"

#include "types.h"

namespace atom {
namespace net {
        

class Subscriber :  virtual public common::IoService
{
public:
    typedef boost::shared_ptr<Subscriber> Pointer;
    
    Subscriber();
    virtual ~Subscriber();
    
protected:
    virtual void SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state) = 0;
    virtual void SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, common::Byteset data) = 0;
    
private:
    void SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state);
    void SlotOnNewData(net::ClientId client_id, net::ServerId server_id, common::Byteset data);
};


        
}; // namespace net
}; // namespace atom
    
#endif // NET_SUBSCRIBER_H
