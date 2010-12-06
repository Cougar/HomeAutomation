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

#ifndef NET_TCPCLIENT_H
#define NET_TCPCLIENT_H

#include "Client.h"

namespace atom {
namespace net {

class TcpClient : public Client
{
public:
    typedef boost::shared_ptr<TcpClient> Pointer;
    typedef  boost::shared_ptr< boost::asio::ip::tcp::acceptor> AcceptorPointer;
    
    TcpClient(boost::asio::io_service& io_service, ClientId id, ServerId server_id);
    virtual ~TcpClient();
    
    void Accept(AcceptorPointer acceptor);
    void Connect(std::string address, unsigned int port);
    void Send(common::Byteset data);
    
    AcceptorPointer ReleaseAcceptor();
    
protected:
    void Read();
    void Stop();
    
private:
    boost::asio::ip::tcp::socket socket_;
    AcceptorPointer acceptor_;
    
    void AcceptHandler(const boost::system::error_code& error);
};

}; // namespace net
}; // namespace atom

#endif // NET_TCPCLIENT_H
