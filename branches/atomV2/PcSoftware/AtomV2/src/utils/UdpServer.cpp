/*
 * UdpServer.cpp
 *
 *  Created on: Jul 20, 2009
 *      Author: Mattias Runge
 */

#include "UdpServer.h"

namespace atom {
namespace utils {

UdpServer::UdpServer(unsigned int port) :
	mySocket(this->myIoService, udp::endpoint(udp::v4(), port))
{
	LOG.setName("UdpServer");

	this->receiveFrom();

	this->start();
}

UdpServer::~UdpServer()
{
	this->stop();
	this->mySocket.cancel();
	this->mySocket.close();
}

UdpServer::pointer UdpServer::getInstance(unsigned int port)
{
	static map<unsigned int, UdpServer::pointer> servers; // FIXME Make thread-safe

    map<unsigned int, UdpServer::pointer>::iterator iter = servers.find(port);

	if (iter != servers.end())
	{
		return iter->second;
	}

	UdpServer::pointer udpServer(new UdpServer(port));

	servers[port] = udpServer;

	return udpServer;
}

udp::endpoint UdpServer::getEndpoint(string address)
{
	LOG.info("Resolving " + address + ":" + convert::int2string(this->getPort()) + "...");
	udp::resolver resolver(this->myIoService);
	udp::resolver::query query(udp::v4(), address, convert::int2string(this->getPort()));
	return *resolver.resolve(query);
}

unsigned int UdpServer::getPort()
{
	return this->mySocket.local_endpoint().port();
}

void UdpServer::sendTo(udp::endpoint receiver, byte_list bytes)
{
	try
	{
		LOG.info("Sending packet, length " + convert::int2string(bytes.size()) + ", to " + receiver.address().to_string() + ":" + convert::int2string(this->getPort()) + "...");
		this->mySocket.send_to(boost::asio::buffer(bytes), receiver);
	}
	catch (std::exception& e)
	{
		LOG.error(e.what());
	}
}

void UdpServer::receiveFrom()
{
	LOG.info("Listening for incoming data on port " + convert::int2string(this->getPort()) + "...");
	this->mySocket.async_receive_from(boost::asio::buffer(this->myDataBuffer, MAX_LENGTH), this->mySender, boost::bind(&UdpServer::handleReceiveFrom, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void UdpServer::handleReceiveFrom(const boost::system::error_code& error, size_t bytesReceived)
{
	if (error)
	{
		LOG.error("handleReceiveFrom got the error code " + convert::int2string(error.value()) + " and the message " + error.message());
	}
	else if (bytesReceived == 0)
	{
		LOG.info("handleReceiveFrom got 0 bytes.");
	}
	else
	{
		LOG.info("Got " + convert::int2string(bytesReceived) + " bytes from " + this->mySender.address().to_string());

		byte_list bytes(this->myDataBuffer, this->myDataBuffer + bytesReceived);

		this->onNewData(this->mySender, bytes);
	}

	this->receiveFrom();
}

void UdpServer::connect(const OnDataSignal::slot_type & slot)
{
	LOG.info("New listener connected.");
	this->onNewData.connect(slot);
}

void UdpServer::run()
{
	LOG.info("Starting thread...");
	this->myIoService.run();
	LOG.info("Thread completed.");
}

}
}
