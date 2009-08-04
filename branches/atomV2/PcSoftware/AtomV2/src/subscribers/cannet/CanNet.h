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
#include <string>
#include "broker/Subscriber.h"
#include "broker/Broker.h"
#include "message/Message.h"
#include "message/Header.h"
#include "utils/BitBuffer.h"
#include "utils/UdpServer.h"
#include "log/Logger.h"
#include "utils/convert.h"
#include "types.h"

namespace atom {
namespace subscribers {

using namespace broker;
using namespace utils;
using namespace message;
using namespace std;

// public boost::signals::trackable,
class CanNet : public Subscriber
{
public:
	typedef boost::shared_ptr<CanNet> pointer;

	CanNet(Broker::pointer broker, string address, unsigned int port);
	virtual ~CanNet();

protected:
	void onNewMessage(Message::pointer message);
	void onNewData(const udp::endpoint & sender, const byte_list & bytes);

	byte_list buildPacket(unsigned int id, byte_list data);
	void processBuffer();
	void sendPing();

	enum
	{
		PACKET_START = 253, PACKET_END = 250, PACKET_PING = 251
	};

	UdpServer::pointer myUdpServer;
	udp::endpoint myEndpoint;
	// TODO Add mutex lock for access to the buffer or are we safe?
	byte_list myBuffer;

private:
	log::Logger LOG;
};

}
}

#endif /* CANNET_H_ */
