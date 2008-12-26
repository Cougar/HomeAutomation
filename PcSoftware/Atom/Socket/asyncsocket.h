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
#include "../SyslogStream/syslogstream.h"

#include "socketexception.h"

#define ASYNCSOCKET_EVENT_NONE 0
#define ASYNCSOCKET_EVENT_DATA 1
#define ASYNCSOCKET_EVENT_CLOSED 2
#define ASYNCSOCKET_EVENT_DIED 3
#define ASYNCSOCKET_EVENT_INACTIVITY 4

const int MAXBUFFER = 1024;

class AsyncSocket : public Thread<AsyncSocket>
{
public:
	AsyncSocket();
	~AsyncSocket();

	void run();

	void setReconnectTimeout(unsigned int timeout);
	void setAddress(string address, int port);

	void startEvent();
	int getEvent();
	void waitForEvent();
	void stopEvent();

	bool availableData();
	string getData();
	bool sendData(string data);
	void forceReconnect();

	static void signalHandler(int signum);
	
protected:
	void reconnectLoop();
	void connect();
	void close();
	void setEvent(int event);

	static Semaphore mySemaphore;

private:
	Semaphore myEventSemaphore;
	int myEvent;

	bool myForceReconnect;
	
	ThreadSafeQueue<string> myInQueue;
	ThreadSafeQueue<string> myOutQueue;

	int mySocket;
	sockaddr_in myAddressStruct;
	string myAddress;
	int myPort;
	unsigned int myReconnectTimeout;
};

#endif	/* _ASYNCSOCKET_H */

