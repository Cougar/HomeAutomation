/*
 * UdpConnection.h
 *
 *  Created on: Jul 17, 2009
 *      Author: Mattias Runge
 */

#ifndef UDPCONNECTION_H_
#define UDPCONNECTION_H_

#include <string>
#include "BitBuffer.h"
#include "Logger.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "../utils/Thread.h"
#include <boost/signal.hpp>

namespace atom {
namespace utils {

using boost::asio::ip::udp;
using namespace std;

class UdpServer
{
public:
	UdpConnection(unsigned int port)
	: socket(this->ioService, udp::endpoint(udp::v4(), port))
	{
		this->socket.async_receive_from(
				boost::asio::buffer(data_, max_length), sender_endpoint_,
				boost::bind(&server::handle_receive_from, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));

	}

private:

	void
	handleReceive(const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
	{
		if (!error || error == boost::asio::error::message_size)
		{
			boost::shared_ptr<std::string>
					message(new std::string(make_daytime_string()));

			socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_, boost::bind(&udp_server::handle_send, this, message, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

			start_receive();
		}
	}

	void
	handle_send(boost::shared_ptr<std::string> /*message*/, const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/)
	{
	}

	udp::socket socket;
	udp::endpoint remoteEndpoint;
	boost::array<char, 1> receiveBuffer;

	io_service ioService;
};

class UdpConnection
{
	typedef boost::signal<void
	(BitBuffer &buffer)> OnIncommingSignal;

public:
	UdpConnection(string ip, unsigned int port);
	virtual
	~UdpConnection();

	void
	connect();
	void
	disconnect();
	void
	write(BitBuffer buffer);

protected:
	string _ip;
	unsigned int _port;
	udp::socket *_outSocket;
	udp::socket *_inSocket;
	udp::endpoint _receiverEndpoint;

private:
	Logger LOG;
};

}
}

#endif /* UDPCONNECTION_H_ */
