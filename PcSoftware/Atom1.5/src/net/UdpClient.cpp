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

#include "UdpClient.h"

#include <boost/lexical_cast.hpp>

#include "common/log.h"

namespace atom {
namespace net {
  
static const std::string log_module_ = "net::udpclient";

UdpClient::UdpClient(boost::asio::io_service& io_service, SocketId id, SocketId server_id) : Client(io_service, id, server_id)
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
}

UdpClient::~UdpClient()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
}

void UdpClient::Connect(std::string address, unsigned int port)
{
  LOG_DEBUG_ENTER;
  
  try
  {
    boost::asio::ip::udp::resolver resolver(this->io_service_);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), address, boost::lexical_cast<std::string>(port));
    boost::asio::ip::udp::resolver::iterator it = resolver.resolve(query);
    
    this->endpoint_ = *it;
    
    this->socket_.reset(new boost::asio::ip::udp::socket(this->io_service_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)));
  }
  catch (std::exception e)
  {
    throw std::runtime_error("Error while connecting to " + address + ":" + boost::lexical_cast<std::string>(port) + ", " + e.what());
  }
  
  this->Read();
  
  LOG_DEBUG_EXIT;
}

void UdpClient::Stop()
{
  LOG_DEBUG_ENTER;
  
  if (this->socket_.use_count() != 0)
  {
    this->socket_->cancel();
    this->socket_->close();
    this->socket_.reset();
  }
  
  LOG_DEBUG_EXIT;
}

void UdpClient::Send(common::Byteset data)
{
  LOG_DEBUG_ENTER;
  
  //log::Debug(log_module_, "data=\"%s\", size=%u", std::string(data.begin(), data.end()).data(), data.size());
  
  try
  {
    if (this->socket_->is_open())
    {
      this->socket_->send_to(boost::asio::buffer(data.data(), data.size()), this->endpoint_);
    }
  }
  catch (std::exception& e)
  {
    this->Disconnect();
    //throw std::runtime_error(e.what());
  }
  
  LOG_DEBUG_EXIT;
}

void UdpClient::Read()
{
  LOG_DEBUG_ENTER;
  
  Client::Read();
  
  //log::Debug(log_module_, "this->buffer_.capacity()=%u", this->buffer_.capacity());

  this->socket_->async_receive(boost::asio::buffer(this->buffer_.data(), this->buffer_.capacity()),
                               boost::bind(&UdpClient::ReadHandler,
                                           this,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
  
  LOG_DEBUG_EXIT;
}

}; // namespace net
}; // namespace atom
