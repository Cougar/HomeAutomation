/*
 * CanNet.cpp
 *
 *  Created on: Apr 27, 2009
 *      Author: Mattias Runge
 */

#include "CanNet.h"
#include <iostream>

using namespace std;

namespace atom {
namespace subscribers {

// TODO Fix input on the form of a URL:
// ex. udp://192.168.1.250:1100
// ex. dev://dev/ttyUSB0:57000
CanNet::CanNet(Broker::pointer broker, string address, unsigned int port) : Subscriber(broker, false)
{
	LOG.setName("CanNetSubscriber");

	this->myUdpServer = net::UdpServer::getInstance(port);
	this->myUdpServer->connect(boost::bind(&CanNet::onNewData, this, _1, _2));

	this->myEndpoint = this->myUdpServer->getEndpoint(address);

	this->sendPing();

	this->start();
}

CanNet::~CanNet()
{
}

byte_list CanNet::buildPacket(unsigned int id, byte_list data)
{
	byte_list packet;

	packet.push_back((unsigned char) PACKET_START);

	packet.push_back((unsigned char) (id & 0xFF));
	packet.push_back((unsigned char) ((id >> 8) & 0xFF));
	packet.push_back((unsigned char) ((id >> 16) & 0xFF));
	packet.push_back((unsigned char) ((id >> 24) & 0xFF));

	packet.push_back((unsigned char) 1); // Ext
	packet.push_back((char) 0); // Rtr

	packet.push_back((unsigned char) data.size());

	for (unsigned char n = 0; n < 8; n++)
	{
		if (n < data.size())
		{
			packet.push_back(data[n]);
		}
		else
		{
			packet.push_back(' ');
		}
	}

	packet.push_back((unsigned char) PACKET_END);

	return packet;
}

void CanNet::processBuffer()
{
	if (this->myBuffer.size() != 15)
	{
		LOG.warn("Received packet of length " + convert::int2string(this->myBuffer.size() + 2) + ", should be 17.");
		this->myBuffer.clear();
		return;
	}

	byte_list data;

	data.push_back(this->myBuffer[0]);
	data.push_back(this->myBuffer[1]);
	data.push_back(this->myBuffer[2]);
	data.push_back(this->myBuffer[3]);

	unsigned char length = this->myBuffer[4];

	for (unsigned char n = 0; n < length; n++)
	{
		data.push_back(this->myBuffer[5 + n]);
	}


	BitBuffer buffer(data);

	Header header;

	header.readBits(buffer);

	// TODO how do we convert from moduleType and messageId to their string names... where are these ids stored?

	/*string moduleType = header.getVariable("moduleType").getValue().
	unsigned int moduleId = header.getVariable("moduleId").getValue().
	string messageType = header.getVariable("messageId").getValue().

	Message::pointer message = boost::make_shared<Message>(moduletype, moduleId, messageType);

	message.readBits(buffer);

	this->put(message);*/

	this->myBuffer.clear();
}

void CanNet::sendPing()
{
	byte_list data(1, (unsigned char) PACKET_PING);

	LOG.info("Sending ping packet...");
	this->myUdpServer->sendTo(this->myEndpoint, data);
}

void CanNet::onNewMessage(Message::pointer message)
{
	LOG.debug("update called");

	BitBuffer buffer;
	// TODO Convert message-variables to bitbuffer then to byte_list
	byte_list data;

	// TODO Convert message-id to long
	unsigned long id;

	this->myUdpServer->sendTo(this->myEndpoint, buildPacket(id, data));
}

void CanNet::onNewData(const udp::endpoint & sender, const byte_list & bytes)
{
	//LOG.debug(udp::endpoint(sender).address().to_string() + "==" + this->myEndpoint.address().to_string());
	//LOG.debug(itos(udp::endpoint(sender).port()) + "==" + itos(this->myEndpoint.port()));
	if (sender.address() == this->myEndpoint.address()) // Port is wrong sometimes, therefor we only compare address
	{
		LOG.debug("Got data.");

		for (unsigned int n = 0; n < bytes.size(); n++)
		{
			if (bytes[n] == PACKET_START)
			{
				this->myBuffer.clear();
			}
			else if (bytes[n] == PACKET_END)
			{
				this->processBuffer();
			}
			else if (bytes[n] == PACKET_PING)
			{
				LOG.info("Received pong.");
			}
			else
			{
				//LOG.info("Received byte. " + itos(bytes[n]));
				this->myBuffer.push_back(bytes[n]);
			}
		}
	}
	else
	{
		LOG.debug("This message was not for me.");
	}
}

}
}
