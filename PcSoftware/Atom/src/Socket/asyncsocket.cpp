/***************************************************************************
 *  Copyright (C) December 6, 2008 by Mattias Runge               *
 *  mattias@runge.se                           *
 *  asyncsocket.cpp                      *
 *                                     *
 *  This program is free software; you can redistribute it and/or modify *
 *  it under the terms of the GNU General Public License as published by *
 *  the Free Software Foundation; either version 2 of the License, or   *
 *  (at your option) any later version.                  *
 *                                     *
 *  This program is distributed in the hope that it will be useful,    *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the     *
 *  GNU General Public License for more details.             *
 *                                     *
 *  You should have received a copy of the GNU General Public License   *
 *  along with this program; if not, write to the             *
 *  Free Software Foundation, Inc.,                    *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
 ***************************************************************************/

#include "socketeventcallback.h"
#include "../Logger/logger.h"

#include "asyncsocket.h"

int socketCount = 1;

AsyncSocket::AsyncSocket()
{
	myId = socketCount++;
	mySocket = -1;
	myReconnectTimeout = 0;
	myForceReconnect = false;
	myEventCallback = NULL;
}

AsyncSocket::~AsyncSocket()
{
	stop();
	silentClose();
}

void AsyncSocket::run()
{
	// If connection is already up then we should not connect again
	if (!isConnected())
	{
		// If we have choosen to not use automatic reconnect do not start reconnect loop
		if (myReconnectTimeout == 0)
		{
			// Connect to somewhere
			connect();
		}
		else
		{
			// Start the reconnect loop
			reconnectLoop();
		}
	}

	bool loop = true;
	try
	{
		while (loop)
		{
			// If we have triggered a forced reconnect do it here
			if (myForceReconnect)
			{
				myForceReconnect = false;
				reconnectLoop();
			}

			// Receive data
			loop = receiveData();
		}
	}
	catch (SocketException *e)
	{
		// Something bad happend and we can not continue
		eventAdd(SocketEvent::TYPE_CONNECTION_DIED, e->getDescription());
	}

	// Clean up socket if we would want to restart
	silentClose();
}

bool AsyncSocket::receiveData()
{
	char buffer[MAXBUFFER + 1];
	memset(buffer, 0, MAXBUFFER + 1);

	int status = ::recv(mySocket, buffer, MAXBUFFER, 0);

	if (status == -1)
	{
		switch (errno)
		{
			case EAGAIN:
			throw new SocketException("The socket is marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.");

			case EBADF:
			throw new SocketException("The argument s is an invalid descriptor.");

			case ECONNREFUSED:
			throw new SocketException("A remote host refused to allow the network connection (typically because it is not running the requested service).");

			case EFAULT:
			throw new SocketException("The receive buffer pointer(s) point outside the process's address space.");

			case EINTR:
			throw new SocketException("The receive was interrupted by delivery of a signal before any data were available; see signal(7).");

			case EINVAL:
			throw new SocketException("Invalid argument passed.");

			case ENOMEM:
			throw new SocketException("Could not allocate memory for recvmsg().");

			case ENOTCONN:
			throw new SocketException("The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).");

			case ENOTSOCK:
			throw new SocketException("The argument s does not refer to a socket.");

			case ECONNRESET:
			eventAdd(SocketEvent::TYPE_CONNECTION_RESET);

			// If we have automatic reconnect we want to start it now
			if (myReconnectTimeout == 0)
			{
				reconnectLoop();
			}
			else
			{
				// Otherwise we would like to end the main loop
				return false;
			}
			break;

			default:
			throw new SocketException("Unknow exception: " + itos(errno));
			break;
		}
	}
	else if (status == 0)
	{
		// Remote host have done a normal shutdown
		eventAdd(SocketEvent::TYPE_CONNECTION_CLOSED);

		if (myReconnectTimeout > 0)
		{
			reconnectLoop();
		}
		else
		{
			return false;
		}
	}
	else if (status > 0)
	{
		// We have received data
		eventAdd(SocketEvent::TYPE_DATA, buffer);
	}

	return true;
}

void AsyncSocket::silentClose()
{
	if (mySocket != -1)
	{
		::close(mySocket);
		mySocket = -1;
	}
}

void AsyncSocket::close()
{
	silentClose();
	eventAdd(SocketEvent::TYPE_CONNECTION_CLOSED);
}

void AsyncSocket::reconnectLoop()
{
	while (true)
	{
		try
		{
			connect();
			return;
		}
		catch (SocketException *e)
		{
			eventAdd(SocketEvent::TYPE_CONNECTION_FAILED, e->getDescription());
			eventAdd(SocketEvent::TYPE_WAITING_RECONNECT);
			sleep(myReconnectTimeout);
		}
	}
}

void AsyncSocket::create()
{
	silentClose();

	mySocket = ::socket(AF_INET, SOCK_STREAM, 0);

	int on = 1;
	int status = setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
	if (status == -1)
	{
		silentClose();
		throw new SocketException("Create:Reuseaddress: " + itos(errno));
	}
}

void AsyncSocket::startListen()
{
	create();

	myAddressStruct.sin_family = AF_INET;
	myAddressStruct.sin_addr.s_addr = INADDR_ANY;
	myAddressStruct.sin_port = htons(myPort);

	int status = ::bind(mySocket, (struct sockaddr *)&myAddressStruct, sizeof(myAddressStruct));

	if (status == -1)
	{
		silentClose();
		switch (errno)
		{
			case EACCES:
			throw new SocketException("The address is protected, and the user is not the superuser.");

			case EADDRINUSE:
			throw new SocketException("The given address is already in use.");

			case EBADF:
			throw new SocketException("sockfd is not a valid descriptor.");

			case EINVAL:
			throw new SocketException("The socket is already bound to an address.");

			case ENOTSOCK:
			throw new SocketException("sockfd is a descriptor for a file, not a socket.");

			//case EACCES:
			//throw new SocketException("Search permission is denied on a component of the path prefix. (See also path_resolution(7).)");

			case EADDRNOTAVAIL:
			throw new SocketException("A nonexistent interface was requested or the requested address was not local.");

			case EFAULT:
			throw new SocketException("addr points outside the user's accessible address space.");

			//case EINVAL:
			//throw new SocketException("The addrlen is wrong, or the socket was not in the AF_UNIX family.");

			case ELOOP:
			throw new SocketException("Too many symbolic links were encountered in resolving addr.");

			case ENAMETOOLONG:
			throw new SocketException("addr is too long.");

			case ENOENT:
			throw new SocketException("The file does not exist.");

			case ENOMEM:
			throw new SocketException("Insufficient kernel memory was available.");

			case ENOTDIR:
			throw new SocketException("A component of the path prefix is not a directory.");

			case EROFS:
			throw new SocketException("The socket inode would reside on a read-only file system.");

			default:
			throw new SocketException("Unknow exception: " + itos(errno));
			break;
		}
	}

	status = ::listen(mySocket, MAXCONNECTIONS);

	if (status == -1)
	{
		silentClose();
		switch (errno)
		{
			case EADDRINUSE:
			throw new SocketException("Another socket is already listening on the same port.");

			case EBADF:
			throw new SocketException("The argument sockfd is not a valid descriptor.");

			case ENOTSOCK:
			throw new SocketException("The argument sockfd is not a socket.");

			case EOPNOTSUPP:
			throw new SocketException("The socket is not of a type that supports the listen() operation.");

			default:
			throw new SocketException("Unknow exception: " + itos(errno));
			break;
		}
	}
}

bool AsyncSocket::accept(AsyncSocket* newSocket)
{
	int addr_length = sizeof(myAddressStruct);
	int socket = ::accept(mySocket, (sockaddr*)&myAddressStruct, (socklen_t*)&addr_length);

	if (socket > 0)
	{
		newSocket->setSocket(socket);
		return true;
	}

	return false;
}

void AsyncSocket::connect()
{
	eventAdd(SocketEvent::TYPE_CONNECTING);

	create();

	memset(&myAddressStruct, 0, sizeof(myAddressStruct));

	myAddressStruct.sin_family = AF_INET;
	myAddressStruct.sin_port = htons(myPort);

	struct hostent *hptr = gethostbyname(myAddress.c_str());
	if (hptr == NULL)
	{
		silentClose();
		throw new SocketException("Connect: Could not resolv ip address");
	}

	memcpy(&myAddressStruct.sin_addr, hptr->h_addr, hptr->h_length);

	int status = ::connect(mySocket, (sockaddr*)&myAddressStruct, sizeof(myAddressStruct));

	if (status == -1)
	{
		silentClose();
		switch (errno)
		{
			case EACCES:
			throw new SocketException("For Unix domain sockets, which are identified by pathname: Write permission is denied on the socket file, or search per- mission is denied for one of the directories in the path prefix. (See also path_resolution(7).)");

			case EPERM:
			throw new SocketException("The user tried to connect to a broadcast address without having the socket broadcast flag enabled or the connection request failed because of a local firewall rule.");

			case EADDRINUSE:
			throw new SocketException("Local address is already in use.");

			case EAFNOSUPPORT:
			throw new SocketException("The passed address didn't have the correct address family in its sa_family field.");

			case EAGAIN:
			throw new SocketException("No more free local ports or insufficient entries in the routing cache. For AF_INET see the net.ipv4.ip_local_port_range sysctl in ip(7) on how to increase the number of local ports.");

			case EALREADY:
			throw new SocketException("The socket is non-blocking and a previous connection attempt has not yet been completed.");

			case EBADF:
			throw new SocketException("The file descriptor is not a valid index in the descriptor table.");

			case ECONNREFUSED:
			throw new SocketException("No-one listening on the remote address.");

			case EFAULT:
			throw new SocketException("The socket structure address is outside the user's address space.");

			case EINPROGRESS:
			throw new SocketException("The socket is non-blocking and the connection cannot be completed immediately. It is possible to select(2) or poll(2) for completion by selecting the socket for writing. After select(2) indicates writability, use getsockopt(2) to read the SO_ERROR option at level SOL_SOCKET to determine whether connect() completed successfully (SO_ERROR is zero) or unsuccessfully (SO_ERROR is one of the usual error codes listed here, explaining the reason for the failure).");

			case EINTR:
			throw new SocketException("The system call was interrupted by a signal that was caught; see signal(7).");

			case EISCONN:
			throw new SocketException("The socket is already connected.");

			case ENETUNREACH:
			throw new SocketException("Network is unreachable.");

			case ENOTSOCK:
			throw new SocketException("The file descriptor is not associated with a socket.");

			case ETIMEDOUT:
			throw new SocketException("Timeout while attempting connection. The server may be too busy to accept new connections. Note that for IP sockets the timeout may be very long when syncookies are enabled on the server.");

			default:
			throw new SocketException("Other errors may be generated by the underlying protocol modules. : " + itos(errno));
		}
	}

	eventAdd(SocketEvent::TYPE_CONNECTED);
}

void AsyncSocket::sendData(string data)
{
	mySendMutex.lock();

	int status = ::send(mySocket, data.c_str(), data.size(), 0);
//Logger::getInstance().add("Sent: \"" + data + "\" status was " + itos(status) + "\n");
	mySendMutex.unlock();

	//Logger &log = Logger::getInstance();
	//log.add("Sent: \"" + data + "\" status was " + itos(status) + "\n");

	if (status == -1)
	{
		switch (errno)
		{
			case EACCES:
			throw new SocketException("(For Unix domain sockets, which are identified by pathname) Write permission is denied on the destination socket file, or search permission is denied for one of the directories the path prefix. (See path_resolution(7).)");

			case EAGAIN:
			throw new SocketException("The socket is marked non-blocking and the requested operation would block.");

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
			throw new SocketException("The output queue for a network interface was full. This generally indicates that the interface has stopped sending, but may be caused by transient congestion. (Normally, this does not occur in Linux. Packets are just silently dropped when a device queue overflows.)");

			case ENOMEM:
			throw new SocketException("No memory available.");

			case ENOTCONN:
			throw new SocketException("The socket is not connected, and no target has been given.");

			case ENOTSOCK:
			throw new SocketException("The argument s is not a socket.");

			case EOPNOTSUPP:
			throw new SocketException("Some bit in the flags argument is inappropriate for the socket type.");

			case EPIPE:
			throw new SocketException("The local end has been shut down on a connection oriented socket. In this case the process will also receive a SIGPIPE unless MSG_NOSIGNAL is set.");

			default:
			throw new SocketException("Unknow exception: " + itos(errno));
		}
	}
}

void AsyncSocket::eventAdd(unsigned int eventType)
{
	eventAdd(eventType, "");
}

void AsyncSocket::eventAdd(unsigned int eventType, string eventData)
{
	SocketEvent socketEvent(eventType, eventData);

	if (myEventCallback == NULL)
	{
		myEventQueue.push(socketEvent);
		myEventSemaphore.broadcast();
	}
	else
	{
		myEventCallback->handleEvent(myId, socketEvent);
	}
}

void AsyncSocket::eventSetCallback(SocketEventCallback* eventCallback)
{
	myEventCallback = eventCallback;
}
