/*
 * UdpConnection.cpp
 *
 *  Created on: Jul 17, 2009
 *      Author: Mattias Runge
 */

#include "UdpConnection.h"

namespace atom {
namespace utils {

UdpConnection::UdpConnection(string ip, unsigned int port)
{
	LOG.setName("UdpConnection");

	this->_ip = ip;
	this->_port = port;

	// TODO Start connect/reconnect-loop, we can not block though

}

UdpConnection::~UdpConnection()
{
	this->disconnect();
}

void
UdpConnection::connect()
{
	LOG.info("Connecting...");

	try
	{
		boost::asio::io_service ioService;

		udp::resolver resolver(ioService);

		LOG.info("Resolving " + this->_ip + ":" + itos(this->_port) + "...");

		udp::resolver::query query(udp::v4(), this->_ip, itos(this->_port));

		this->_receiverEndpoint = *resolver.resolve(query);

		LOG.info("Creating socket...");
		this->_outSocket = new udp::socket(ioService);

		LOG.info("Opening socket...");
		this->_outSocket->open(udp::v4());

		LOG.info("Connected.");
	}
	catch (std::exception& e)
	{
		LOG.error(e.what());
	}
}

void
UdpConnection::disconnect()
{
	LOG.info("Disconnecting...");

	if (this->_outSocket != NULL)
	{
		this->_outSocket->close();
		delete this->_outSocket;
		this->_outSocket = NULL;
	}

	if (this->_inSocket != NULL)
	{
		this->_inSocket->close();
		delete this->_inSocket;
		this->_inSocket = NULL;
	}

	LOG.info("Disconnected.");
}

void
UdpConnection::incomming(BitBuffer buffer)
{
	LOG.info("Received packet from socket.");
}

void
UdpConnection::write(BitBuffer buffer)
{
	LOG.info("Writing buffer to socket...");

	try
	{
		this->_outSocket->send_to(boost::asio::buffer(buffer.getAsString()),
				this->_receiverEndpoint);
	}
	catch (std::exception& e)
	{
		LOG.error(e.what());
	}
}

void
UdpConnection::run()
{
	boost::asio::io_service ioService;

	try
	{
		this->_inSocket = new udp::socket(ioService, udp::endpoint(udp::v4(),
				this->_port));

		char data[1024];
		udp::endpoint sender_endpoint;
		BitBuffer buffer;

		while (true)
		{
			size_t length = this->_inSocket->receive_from(boost::asio::buffer(
					data, 1024), sender_endpoint);

			buffer.setFromString(string(data, length));

			this->incomming(buffer);
		}
	}
	catch (std::exception& e)
	{
		LOG.error(e.what());
	}
}

}
}
