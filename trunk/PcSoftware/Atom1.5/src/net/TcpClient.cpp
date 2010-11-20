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

namespace atom {
namespace net {

TcpClient::TcpClient(boost::asio::io_service& io_service, ClientId id, ServerId server_id) : socket_(io_service), Client(io_service, id, server_id)
{
}

TcpClient::~TcpClient()
{
}

void TcpClient::Accept(AcceptorPointer acceptor)
{
    try
    {
        this->acceptor_ = acceptor;
        
        this->acceptor_->async_accept(this->socket_,
                                      boost::bind(&TcpClient::AcceptHandler,
                                                  this,
                                                  boost::asio::placeholders::error));
    }
    catch (std::exception e)
    {
        throw std::runtime_error("Error while opening port, " + std::string(e.what()));
    }
}

void TcpClient::AcceptHandler(const boost::system::error_code& error)
{
    this->Read();
    
    this->signal_on_new_state_(this->GetId(), this->GetServerId(), CLIENT_STATE_ACCEPTED);
}

TcpClient::AcceptorPointer TcpClient::ReleaseAcceptor()
{
    AcceptorPointer acceptor = this->acceptor_;
    this->acceptor_.reset();
    return acceptor;
}

void TcpClient::Connect(std::string address, unsigned int port)
{
    try
    {
        boost::asio::ip::tcp::resolver resolver(this->socket_.get_io_service());
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), address, boost::lexical_cast<std::string>(port));
        boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
        
        this->socket_.connect(*it);
    }
    catch (std::exception e)
    {
        throw std::runtime_error("Error while connecting to " + address + ":" + boost::lexical_cast<std::string>(port));
    }
    
    this->Read();
}

void TcpClient::Stop()
{
    if (this->socket_.is_open())
    {
        this->socket_.cancel();
        this->socket_.close();
    }
    
    if (this->acceptor_.use_count() != 0)
    {
        this->acceptor_->cancel();
        this->acceptor_->close();
    }
}

void TcpClient::Send(type::Byteset data)
{
    if (this->acceptor_.use_count() != 0)
    {
        return;
    }
    
    try
    {
        if (this->socket_.is_open())
        {
            this->socket_.send(boost::asio::buffer(data.Get(), data.GetMaxSize()));
        }
    }
    catch (std::exception& e)
    {
        this->Disconnect();
        //throw std::runtime_error(e.what());
    }
}

void TcpClient::Read()
{
    Client::Read();
    
    this->socket_.async_read_some(boost::asio::buffer(this->buffer_.Get(), this->buffer_.GetMaxSize()),
                                  boost::bind(&TcpClient::ReadHandler,
                                              this,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
}

    
}; // namespace net
}; // namespace atom
