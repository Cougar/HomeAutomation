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

namespace atom {
namespace net {

UdpClient::UdpClient(boost::asio::io_service& io_service, ClientId id, ServerId server_id) : io_service_(io_service), Client(io_service, id, server_id)
{
}

UdpClient::~UdpClient()
{
}

void UdpClient::Connect(std::string address, unsigned int port)
{
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
}

void UdpClient::Stop()
{
    if (this->socket_.use_count() != 0)
    {
        this->socket_->cancel();
        this->socket_->close();
        this->socket_.reset();
    }
}

void UdpClient::Send(common::Byteset data)
{
    try
    {
        if (this->socket_->is_open())
        {
            this->socket_->send_to(boost::asio::buffer(data.Get(), data.GetMaxSize()), this->endpoint_);
        }
    }
    catch (std::exception& e)
    {
        this->Disconnect();
        //throw std::runtime_error(e.what());
    }
}

void UdpClient::Read()
{
    Client::Read();

    this->socket_->async_receive(boost::asio::buffer(this->buffer_.Get(), this->buffer_.GetMaxSize()),
                                 boost::bind(&UdpClient::ReadHandler,
                                             this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
}

}; // namespace net
}; // namespace atom
