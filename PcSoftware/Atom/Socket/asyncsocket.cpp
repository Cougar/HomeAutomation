/***************************************************************************
 *   Copyright (C) December 6, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   asyncsocket.cpp                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "asyncsocket.h"
#include <iostream>
Semaphore AsyncSocket::mySemaphore;

AsyncSocket::AsyncSocket()
{
	mySocket = -1;
	myReconnectTimeout = 10;
	myForceReconnect = false;

	Thread<AsyncSocket>();
}

AsyncSocket::~AsyncSocket()
{
	if (mySocket != -1)
	{
		::close(mySocket);
		mySocket = -1;
	}
	
	stop();
}

void AsyncSocket::run()
{
	SyslogStream &slog = SyslogStream::getInstance();

	reconnectLoop();

	char buf[MAXBUFFER + 1];
	string data;
	int status;
	int rc;
	int timeSince = time(NULL) + 10;
	
	AsyncSocket::mySemaphore.lock();

	try
	{
		while (1)
		{
			rc = mySemaphore.wait(5);

			if (myForceReconnect)
			{
				slog << "Disconnected from server.\n";
				reconnectLoop();
				timeSince = time(NULL) + 10;
				continue;
			}

			//cout << "Socket awoken...\n";
			
			if (rc == ETIMEDOUT)
			{
				/* Socket timed out, this means we have not received anything in some time
				and we should check the connection */

				setEvent(ASYNCSOCKET_EVENT_INACTIVITY);
				timeSince = time(NULL);
			}
			else
			{
				while (myOutQueue.size() > 0)
				{
					data = myOutQueue.pop();

					//slog << "Sending: " << data << "\n";

					status = ::send(mySocket, data.c_str(), data.size(), 0);

					//slog << "Status: " << status << "\n";

					if (status == -1)
					{
						switch (errno)
						{
							case EACCES:
							throw new SocketException("(For  Unix  domain sockets, which are identified by pathname) Write permission is denied on the destination socket file, or search permission is denied for one of the directories the path prefix.  (See path_resolution(7).)");

							case EAGAIN:
							//slog << "The socket is marked non-blocking and the requested operation would block.\n";
							break;

							case EBADF:
							throw new SocketException("An invalid descriptor was specified.");

							case ECONNRESET:
							throw new SocketException("Connection reset by peer.");

							case EDESTADDRREQ:
							throw new SocketException("The socket is not connection-mode, and no peer address is set.");

							case EFAULT:
							throw new SocketException("An invalid user space address was specified for an argument.");

							case EINTR:
							throw new SocketException("A signal occurred before any data was transmitted; see signal(7).");

							case EINVAL:
							throw new SocketException("1Invalid argument passed.");

							case EISCONN:
							throw new SocketException("The connection-mode socket was connected already but a recipient was specified. (Now either this error is returned, or the recipient specification is ignored.)");

							case EMSGSIZE:
							throw new SocketException("The socket type requires that message be sent atomically, and the size of the message to be sent made this impossible.");

							case ENOBUFS:
							throw new SocketException("The output queue for a network interface was full.  This generally indicates that the interface has stopped sending, but may be caused by transient congestion.  (Normally, this does not occur in Linux.  Packets are just silently dropped when a device queue overflows.)");

							case ENOMEM:
							throw new SocketException("No memory available.");

							case ENOTCONN:
							throw new SocketException("The socket is not connected, and no target has been given.");

							case ENOTSOCK:
							throw new SocketException("The argument s is not a socket.");

							case EOPNOTSUPP:
							throw new SocketException("Some bit in the flags argument is inappropriate for the socket type.");

							case EPIPE:
							throw new SocketException("The  local end has been shut down on a connection oriented socket. In this case the process will also receive a SIGPIPE unless MSG_NOSIGNAL is set.");

							default:
							slog << "Unknow exception: " << errno << "\n";
							break;
						}
					}
				}

				memset(buf, 0, MAXBUFFER + 1);

				status = ::recv(mySocket, buf, MAXBUFFER, 0);

				//slog << "Receiving: " << buf << "\n";

				if (status == -1)
				{
					switch (errno)
					{
						case EAGAIN:
						//slog << "The socket is marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.\n";
						break;

						case EBADF:
						throw new SocketException("The argument s is an invalid descriptor.");

						case ECONNREFUSED:
						throw new SocketException("A remote host refused to allow the network connection (typically because it is not running the requested service).");

						case EFAULT:
						throw new SocketException("The receive buffer pointer(s) point outside the process's address space.");

						case EINTR:
						throw new SocketException("The receive was interrupted by delivery of a signal before any data were available; see signal(7).");

						case EINVAL:
						//throw new SocketException("2Invalid argument passed.");
						slog << "Disconnected from server.\n";
						reconnectLoop();
						break;

						case ENOMEM:
						throw new SocketException("Could not allocate memory for recvmsg().");

						case ENOTCONN:
						throw new SocketException("The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and  accept(2)).");

						case ENOTSOCK:
						throw new SocketException("The argument s does not refer to a socket.");

						default:
						slog << "Unknow exception: " << errno << "\n";
						break;
					}
				}
				else if (status == 0)
				{
					slog << "Disconnected from server.\n";
					reconnectLoop();
				}
				else if (status > 0)
				{
					data = buf;

					myInQueue.push(data);

					setEvent(ASYNCSOCKET_EVENT_DATA);
					
					timeSince = time(NULL);
				}
				
				if (timeSince + 10 < time(NULL))
				{
					setEvent(ASYNCSOCKET_EVENT_INACTIVITY);
					timeSince = time(NULL);
				}
			}
		}
	}
	catch (SocketException *e)
	{
		slog << "Exception: " << e->getDescription() << "\n";
		mySemaphore.unlock();
		setEvent(ASYNCSOCKET_EVENT_DIED);
		stop();
	}

	close();

	mySemaphore.unlock();
}

void AsyncSocket::reconnectLoop()
{
	SyslogStream &slog = SyslogStream::getInstance();

	while (1)
	{
		try
		{
			slog << "Trying to connect...\n";
			connect();
			slog << "Connection established.\n";
			break;
		}
		catch (SocketException *e)
		{
			slog << "Could not connect: " << e->getDescription() << "\n";
			slog << "Will try again in " << myReconnectTimeout << " seconds\n";
			sleep(myReconnectTimeout);
		}
	}
}

void AsyncSocket::connect()
{
	if (mySocket != -1)
	{
		::close(mySocket);
		mySocket = -1;
	}
	
	mySocket = ::socket(AF_INET, SOCK_STREAM, 0);

	int on = 1;
	int status = setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
	if (status == -1)
	{
		throw new SocketException("Connect:Reuseaddress: " + itos(errno));
	}

	///FIXME: Verify that this works
	status = setsockopt(mySocket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&on, sizeof(on));
	if (status == -1)
	{
		throw new SocketException("Connect:Keepalive: " + itos(errno));
	}

	///FIXME: Verify that this works
	status = setsockopt(mySocket, SOL_TCP, TCP_KEEPIDLE, (const char*)&on, sizeof(on));
	if (status == -1)
	{
		throw new SocketException("Connect:Keepidle: " + itos(errno));
	}

	memset(&myAddressStruct, 0, sizeof(myAddressStruct));

	myAddressStruct.sin_family = AF_INET;
	myAddressStruct.sin_port = htons(myPort);

	status = inet_pton(AF_INET, myAddress.c_str(), &myAddressStruct.sin_addr);

	if (status == -1)
	{
		if (errno == EAFNOSUPPORT)
			throw new SocketException("Connect: EAFNOSUPPORT");
	}
	
	status = ::connect(mySocket, (sockaddr*)&myAddressStruct, sizeof(myAddressStruct));

	if (status == -1)
	{
		switch (errno)
		{
			case EACCES:
			throw new SocketException("For Unix domain sockets, which are identified by pathname: Write permission is denied on the socket file, or search per- mission is denied for one of the directories in the path prefix.  (See also path_resolution(7).)");

			case EPERM:
			throw new SocketException("The user tried to connect to a broadcast address without having the socket broadcast  flag  enabled  or  the  connection request failed because of a local firewall rule.");

			case EADDRINUSE:
			throw new SocketException("Local address is already in use.");

			case EAFNOSUPPORT:
			throw new SocketException("The passed address didn't have the correct address family in its sa_family field.");

			case EAGAIN:
			throw new SocketException("No more free local ports or insufficient entries in the routing cache.  For AF_INET see the net.ipv4.ip_local_port_range sysctl in ip(7) on how to increase the number of local ports.");

			case EALREADY:
			throw new SocketException("The socket is non-blocking and a previous connection attempt has not yet been completed.");

			case EBADF:
			throw new SocketException("The file descriptor is not a valid index in the descriptor table.");

			case ECONNREFUSED:
			throw new SocketException("No-one listening on the remote address.");

			case EFAULT:
			throw new SocketException("The socket structure address is outside the user's address space.");

			case EINPROGRESS:
			throw new SocketException("The socket is non-blocking and the connection cannot be completed immediately.  It is possible to select(2)  or  poll(2) for  completion  by  selecting the socket for writing.  After select(2) indicates writability, use getsockopt(2) to read the SO_ERROR option at level SOL_SOCKET to determine whether connect() completed  successfully  (SO_ERROR  is  zero)  or unsuccessfully (SO_ERROR is one of the usual error codes listed here, explaining the reason for the failure).");

			case EINTR:
			throw new SocketException("The system call was interrupted by a signal that was caught; see signal(7).");

			case EISCONN:
			throw new SocketException("The socket is already connected.");

			case ENETUNREACH:
			throw new SocketException("Network is unreachable.");

			case ENOTSOCK:
			throw new SocketException("The file descriptor is not associated with a socket.");

			case ETIMEDOUT:
			throw new SocketException("Timeout  while  attempting  connection.  The server may be too busy to accept new connections.  Note that for IP sockets the timeout may be very long when syncookies are enabled on the server.");

			default:
			throw new SocketException("Other errors may be generated by the underlying protocol modules. : " + itos(errno));
		}
	}

	struct sigaction saio;
	saio.sa_handler = AsyncSocket::signalHandler;
	sigemptyset(&saio.sa_mask);
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO, &saio, NULL);

	fcntl(mySocket, F_SETOWN, getpid());
	int flags = fcntl(mySocket, F_GETFL);

	if (flags < 0)
		throw new SocketException("Async socket fcntl failed");

	fcntl(mySocket, F_SETFL, flags | O_NONBLOCK | FASYNC);

	myForceReconnect = false;
}

void AsyncSocket::close()
{
	if (mySocket != -1)
	{
		::close(mySocket);
		mySocket = -1;
		setEvent(ASYNCSOCKET_EVENT_CLOSED);
	}
}

void AsyncSocket::setReconnectTimeout(unsigned int timeout)
{
	myReconnectTimeout = timeout;
}

void AsyncSocket::startEvent()
{
	myEventSemaphore.lock();
}

int AsyncSocket::getEvent()
{
	int event = myEvent;
	myEvent = ASYNCSOCKET_EVENT_NONE;
	return event;
}

void AsyncSocket::waitForEvent()
{
	myEventSemaphore.wait();
}

void AsyncSocket::stopEvent()
{
	myEventSemaphore.unlock();
}

void AsyncSocket::setAddress(string address, int port)
{
	myAddress = address;
	myPort = port;
}

bool AsyncSocket::availableData()
{
	return (myInQueue.size() > 0);
}

string AsyncSocket::getData()
{
	return myInQueue.pop();
}

bool AsyncSocket::sendData(string data)
{
	myOutQueue.push(data);
	mySemaphore.broadcast();
	return true;
}

void AsyncSocket::setEvent(int event)
{
	myEventSemaphore.lock();
	myEvent = event;
	myEventSemaphore.unlock();
	myEventSemaphore.broadcast();
}

void AsyncSocket::forceReconnect()
{
	myForceReconnect = true;
	mySemaphore.broadcast();
}

void AsyncSocket::signalHandler(int signum)
{
	//FIXME: We must know which socket is ready to read by using select... 
	//cout << "DEBUG: signalHandler signum: " << signum << endl;
	mySemaphore.broadcast();
}
