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

#ifndef NET_TCPSERVER_H
#define NET_TCPSERVER_H

#include <stdint.h>
#include <boost/bind.hpp>
#include <boost/cast.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>

#include "types.h"

namespace atom {
namespace net {

class TcpServer
{
public:
  typedef boost::shared_ptr<TcpServer>                              Pointer;
  typedef boost::signals2::signal<void(SocketId, TcpSocketPointer)> SignalOnNewClient;
  
  TcpServer(boost::asio::io_service& io_service, unsigned int port, SocketId id);
  ~TcpServer();

  void ConnectSlots(const SignalOnNewClient::slot_type& slot_on_new_client);
  void Accept();
  SocketId GetId();

private:
  TcpSocketPointer                new_client_socket_;
  boost::asio::ip::tcp::acceptor  acceptor_;
  SocketId                        id_;
  uint32_t                        port_;
  SignalOnNewClient               signal_on_new_client_;
  
  void AcceptHandler(const boost::system::error_code& error);
};

}; // namespace net
}; // namespace atom

#endif // NET_TCPCLIENT_H
