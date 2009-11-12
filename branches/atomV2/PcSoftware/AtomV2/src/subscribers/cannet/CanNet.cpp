/*
 * CanNet.cpp
 *
 *  Created on: Apr 27, 2009
 *      Author: Mattias Runge
 */

#include "CanNet.h"

#include <iostream>

namespace atom {
namespace subscribers {

// TODO Fix input on the form of a URL:
// ex. udp://192.168.1.250:1100
// ex. dev://dev/ttyUSB0:57000
CanNet::CanNet(string address, unsigned int port) : Subscriber(false)
{
	LOG.setName("CanNetSubscriber");

	this->udp_server = net::UdpServer::getInstance(port);
	this->udp_server->connect(boost::bind(&CanNet::Slot_OnNewData, this, _1, _2));

	this->endpoint_ = this->udp_server->getEndpoint(address);

	this->SendPing();

	this->Start();
}

CanNet::~CanNet()
{
}

ByteList CanNet::BuildPacket(unsigned int id, ByteList data)
{
	ByteList packet;

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

void CanNet::SendPing()
{
	ByteList data(1, (unsigned char) PACKET_PING);

	LOG.info("Sending ping packet...");
	this->myUdpServer->sendTo(this->myEndpoint, data);
}

void CanNet::ProcessBuffer()
{
	if (this->buffer_.size() != 15)
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

	Header header(buffer);

	Message::pointer message = boost::make_shared<Message>(header);

	message->readBits(buffer);

	this->put(message);

	this->myBuffer.clear();
}

void CanNet::OnMessage(Message::Pointer message)
{
	LOG.debug("update called");

	BitBuffer buffer;

	message->getHeader().writeBits(buffer);

	unsigned long id;
	buffer.read(32, id);

	buffer.clear();

	message->writeBits(buffer);

	byte_list data = buffer.getAsBytes();

	this->myUdpServer->sendTo(this->myEndpoint, buildPacket(id, data));
}

void CanNet::Slot_OnNewData(const udp::endpoint &sender, const ByteList &bytes)
{
	//LOG.debug(udp::endpoint(sender).address().to_string() + "==" + this->myEndpoint.address().to_string());
	//LOG.debug(itos(udp::endpoint(sender).port()) + "==" + itos(this->myEndpoint.port()));
	if (sender.address() == this->enporint_.address()) // Port is wrong sometimes, therefor we only compare address
	{
		LOG.debug("Got data.");

		for (unsigned int n = 0; n < bytes.size(); n++)
		{
			if (bytes[n] == PACKET_START)
			{
				this->buffer_.clear();
			}
			else if (bytes[n] == PACKET_END)
			{
				this->ProcessBuffer();
			}
			else if (bytes[n] == PACKET_PING)
			{
				LOG.info("Received pong.");
			}
			else
			{
				//LOG.info("Received byte. " + itos(bytes[n]));
				this->buffer_.push_back(bytes[n]);
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
