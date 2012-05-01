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

#ifndef NET_SERIALCLIENT_H
#define NET_SERIALCLIENT_H

#include "Client.h"

namespace atom {
namespace net {

class SerialClient : public Client
{
public:
    typedef boost::shared_ptr<SerialClient> Pointer;
    
    SerialClient(boost::asio::io_service& io_service, SocketId id, SocketId server_id);
    virtual ~SerialClient();
    
    void Connect(std::string address, unsigned int baud);
    void Send(common::Byteset data);
    
protected:
    void Read();
    void Stop();
    
private:
    boost::asio::serial_port serial_port_;
};

}; // namespace net
}; // namespace atom

#endif // NET_SERIALCLIENT_H
