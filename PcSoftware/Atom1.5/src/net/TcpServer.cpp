/*

    Copyright (C) 2010  Mattias Runge

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "TcpServer.h"

#include "common/log.h"

namespace atom {
namespace net {

static const std::string log_module_ = "net::tcpserver";
  
TcpServer::TcpServer(boost::asio::io_service& io_service, unsigned int port, SocketId id) : acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
  LOG_DEBUG_ENTER;
  
  this->port_ = port;
  this->id_   = id;
  
  LOG_DEBUG_EXIT;
}

TcpServer::~TcpServer()
{
  LOG_DEBUG_ENTER;
  
  if (this->new_client_socket_.use_count() != 0)
  {
    if (this->new_client_socket_->is_open())
    {
      this->new_client_socket_->cancel();
      this->new_client_socket_->close();
    }
    
    this->new_client_socket_.reset();
  }
    
  this->acceptor_.cancel();
  this->acceptor_.close();
  
  LOG_DEBUG_EXIT;
}

void TcpServer::Accept()
{
  LOG_DEBUG_ENTER;
  
  this->new_client_socket_ = TcpSocketPointer(new boost::asio::ip::tcp::socket(this->acceptor_.get_io_service()));
  
  this->acceptor_.async_accept(*this->new_client_socket_,
                               boost::bind(&TcpServer::AcceptHandler,
                                           this,
                                           boost::asio::placeholders::error));
  
  LOG_DEBUG_EXIT;
}

void TcpServer::AcceptHandler(const boost::system::error_code& error)
{
  LOG_DEBUG_ENTER;
  
  this->signal_on_new_client_(this->id_, this->new_client_socket_);
  
  this->new_client_socket_.reset();
  
  this->Accept();
  
  LOG_DEBUG_EXIT;
}

SocketId TcpServer::GetId()
{
  LOG_DEBUG_ENTER;
  
  LOG_DEBUG_EXIT;
  
  return this->id_;
}

void TcpServer::ConnectSlots(const SignalOnNewClient::slot_type& slot_on_new_client)
{
  LOG_DEBUG_ENTER;
  
  this->signal_on_new_client_.connect(slot_on_new_client);
  
  LOG_DEBUG_EXIT;
}


}; // namespace net
}; // namespace atom
