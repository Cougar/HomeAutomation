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

#include "TcpClient.h"

#include <boost/lexical_cast.hpp>

#include "common/log.h"

namespace atom {
namespace net {

static const std::string log_module_ = "net::tcpclient";
  
TcpClient::TcpClient(boost::asio::io_service& io_service, SocketId id, SocketId server_id) : Client(io_service, id, server_id)
{
  LOG_DEBUG_ENTER;
  
  LOG_DEBUG_EXIT;
}
  
TcpClient::TcpClient(boost::asio::io_service& io_service, TcpSocketPointer socket, SocketId id, SocketId server_id) : Client(io_service, id, server_id)
{
  LOG_DEBUG_ENTER;
  
  this->socket_ = socket;
  
  this->Read();
  
  LOG_DEBUG_EXIT;
}

TcpClient::~TcpClient()
{
  LOG_DEBUG_ENTER;
  
  LOG_DEBUG_EXIT;
}

void TcpClient::Connect(std::string address, unsigned int port)
{
  LOG_DEBUG_ENTER;
  
  this->socket_ = TcpSocketPointer(new boost::asio::ip::tcp::socket(this->io_service_));
  
  boost::asio::ip::tcp::resolver resolver(this->socket_->get_io_service());
  boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), address, boost::lexical_cast<std::string>(port));
  boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);

  this->socket_->connect(*it);

  this->Read();
  
  LOG_DEBUG_EXIT;
}

void TcpClient::Send(common::Byteset data)
{
  LOG_DEBUG_ENTER;
  
  //log::Debug(log_module_, "data=\"%s\", size=%u", std::string(data.begin(), data.end()).data(), data.size());
  
  try
  {
    if (this->socket_->is_open())
    {
      this->socket_->send(boost::asio::buffer(data.data(), data.size()));
    }
  }
  catch (std::exception& e)
  {
    this->Disconnect();
      //throw std::runtime_error(e.what());
  }
  
  LOG_DEBUG_EXIT;
}

void TcpClient::Read()
{
  LOG_DEBUG_ENTER;
  
  Client::Read();
  
  //log::Debug(log_module_, "this->buffer_.capacity()=%u", this->buffer_.capacity());
    
  this->socket_->async_read_some(boost::asio::buffer(this->buffer_.data(), this->buffer_.capacity()),
                                 boost::bind(&TcpClient::ReadHandler,
                                             this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
  
  LOG_DEBUG_EXIT;
}


}; // namespace net
}; // namespace atom
