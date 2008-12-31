/***************************************************************************
 *   Copyright (C) December 6, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   asyncsocket.h                                            *
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

#ifndef _ASYNCSOCKET_H
#define	_ASYNCSOCKET_H

using namespace std;

#include <string>
#include <queue>
#include <map>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <netinet/tcp.h>


#include "../Threads/thread.h"
#include "../Threads/semaphore.h"
#include "../Threads/threadsafequeue.h"
#include "../Tools/tools.h"

#include "socketexception.h"
#include "socketevent.h"

const int MAXBUFFER = 1024;
const int MAXCONNECTIONS = 10;

class AsyncSocket : public Thread<AsyncSocket>
{
public:
	AsyncSocket();
	~AsyncSocket();

	void run();

	string getId() { return myId; };
	bool isConnected() { return mySocket != -1; };

	void forceReconnect() { myForceReconnect = true; };
	void setReconnectTimeout(unsigned int reconnectTimeout) { myReconnectTimeout = reconnectTimeout; };
	unsigned int getReconnectTimeout() { return myReconnectTimeout; };
	void setAddress(string address) { myAddress = address; };
	string getAddress() { return myAddress; };
	void setPort(int port) { myPort = port; };
	int getPort() { return myPort; };
	void setSocket(int socket) { mySocket = socket; };
	int getSocket() { return mySocket; };

	void eventStartListen() { myEventSemaphore.lock(); };
	void eventWait() { myEventSemaphore.wait(); };
	void eventStopListen() { myEventSemaphore.unlock(); };
	bool eventIsAvailable() { return myEventQueue.size() > 0; };
	SocketEvent eventFetch() { return myEventQueue.pop(); };


	void sendData(string data);

	
	void startListen();
	bool accept(AsyncSocket* newSocket);
	

	

protected:
	void reconnectLoop();
	void create();

	void connect();
	
	void silentClose();
	void close();


private:
	string myId;
	string myAddress;
	int myPort;
	unsigned int myReconnectTimeout;
	int mySocket;
	bool myForceReconnect;
	sockaddr_in myAddressStruct;

	Semaphore myEventSemaphore;
	ThreadSafeQueue<SocketEvent> myEventQueue;
	void eventAdd(unsigned int eventType, string eventData);
	void eventAdd(unsigned int eventType);

	bool receiveData();
};

#endif	/* _ASYNCSOCKET_H */

