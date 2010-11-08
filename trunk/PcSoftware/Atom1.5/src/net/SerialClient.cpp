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

#include "SerialClient.h"

#include <boost/lexical_cast.hpp>

namespace atom {
namespace net {

SerialClient::SerialClient(boost::asio::io_service& io_service, ClientId id, ServerId server_id) : serial_port_(io_service), Client(io_service, id, server_id)
{
}

SerialClient::~SerialClient()
{
    if (this->serial_port_.is_open())
    {
        this->serial_port_.cancel();
        this->serial_port_.close();
    }
}

void SerialClient::Connect(std::string address, unsigned int baud)
{
    boost::asio::serial_port_base::baud_rate baud_option(baud);
    
    this->serial_port_.open(address);
    
    if (!this->serial_port_.is_open())
    {
        throw std::runtime_error("Error while opening " + address);
    }
    
    this->serial_port_.set_option(baud_option);
    
    this->Read();
}

void SerialClient::Disconnect()
{
    if (this->serial_port_.is_open())
    {
        this->serial_port_.cancel();
        this->serial_port_.close();
    }
    
    Client::Disconnect();
}

void SerialClient::Send(Buffer data)
{
    if (this->serial_port_.is_open())
    {
        this->serial_port_.write_some(boost::asio::buffer(data));
    }
    else
    {
        this->Disconnect();
    }
}

void SerialClient::Read()
{
    Client::Read();
    
    this->serial_port_.async_read_some(boost::asio::buffer(this->buffer_),
                                       boost::bind(&SerialClient::ReadHandler,
                                                   this,
                                                   boost::asio::placeholders::error,
                                                   boost::asio::placeholders::bytes_transferred));
}


}; // namespace net
}; // namespace atom
