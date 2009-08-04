/*
 * UdpServer.h
 *
 *  Created on: Jul 20, 2009
 *      Author: Mattias Runge
 */

#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/signal.hpp>
#include <boost/make_shared.hpp>
#include "log/Logger.h"
#include "utils/BitBuffer.h"
#include "thread/Thread.h"
#include "utils/convert.h"
#include "types.h"

namespace atom {
namespace net {

using boost::asio::ip::udp;
using namespace utils;

class UdpServer : protected thread::Thread
{
	typedef boost::signal<void(udp::endpoint & sender, byte_list & bytes)> OnDataSignal;

public:
	typedef boost::shared_ptr<UdpServer> pointer;

	~UdpServer();

	static pointer getInstance(unsigned int port);

	void sendTo(udp::endpoint receiver, byte_list bytes);
	void connect(const OnDataSignal::slot_type & slot);

	udp::endpoint getEndpoint(string address);
	unsigned int getPort();

private:
	UdpServer(unsigned int port);

	void run();
	void receiveFrom();
	void handleReceiveFrom(const boost::system::error_code& error, size_t bytes_received);

	log::Logger LOG;
	OnDataSignal onNewData;

	enum { MAX_LENGTH = 64 };

	boost::asio::io_service myIoService;
	udp::socket mySocket;

	udp::endpoint mySender;
	unsigned char myDataBuffer[MAX_LENGTH];
};

}
}

#endif /* UDPSERVER_H_ */
