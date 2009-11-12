/*
 * CanNet.h
 *
 *  Created on: Apr 27, 2009
 *      Author: Mattias Runge
 */

#ifndef CANNET_H_
#define CANNET_H_

#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/signal.hpp>
#include <boost/asio.hpp>
#include <string>
#include "broker/Subscriber.h"
#include "broker/Broker.h"
#include "message/Message.h"
#include "message/Header.h"
#include "message/BitBuffer.h"
#include "net/UdpServer.h"
#include "log/Logger.h"
#include "utils/convert.h"
#include "types.h"

namespace atom {
namespace subscribers {

// public boost::signals::trackable,
class CanNet : public Subscriber
{
public:
	typedef boost::shared_ptr<CanNet> Pointer;

	CanNet(std::string address, unsigned int port);
	virtual ~CanNet();

private:
	log::Logger LOG;

	void OnMessage(Message::Pointer message);
	void Slot_OnNewData(const udp::endpoint &sender, const ByteList &bytes);

	ByteList BuildPacket(unsigned int id, ByteList data);
	void ProcessBuffer();
	void SendPing();

	enum
	{
		PACKET_START = 253, PACKET_END = 250, PACKET_PING = 251
	};

	net::UdpServer::pointer udp_server_;
	boost::asio::ip::udp::endpoint endpoint_;

	// TODO Add mutex lock for access to the buffer or are we safe?
	ByteList buffer_;
};

} // namespace subscribers
} // namespace atom

#endif /* CANNET_H_ */
