/***************************************************************************
 *   Copyright (C) December 26, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   socketthread.cpp                                            *
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

#include "socketthread.h"
#include "virtualmachine.h"

SocketThread::SocketThread(string address, int port, unsigned int reconnectTimeout)
{
	myId = time(NULL);

	mySocket.setAddress(address, port);
	mySocket.setReconnectTimeout(reconnectTimeout);

	Thread<SocketThread>();
}

SocketThread::~SocketThread()
{
	stop();
}

void SocketThread::run()
{
	SyslogStream &slog = SyslogStream::getInstance();
	VirtualMachine &vm = VirtualMachine::getInstance();

	mySocket.start();

	mySocket.startEvent();

	while (1)
	{
		mySocket.waitForEvent();

		int event = mySocket.getEvent();

		if (event == ASYNCSOCKET_EVENT_DATA)
		{
			while (mySocket.availableData())
			{
				string data = mySocket.getData();
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'EVENT_DATA', '" + escape(data) + "');");
			}
		}
		else if (event == ASYNCSOCKET_EVENT_CLOSED)
		{
			vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'EVENT_CLOSED', '');");
		}
		else if (event == ASYNCSOCKET_EVENT_DIED)
		{
			vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'EVENT_DIED', '');");
			break;
		}
		else if (event == ASYNCSOCKET_EVENT_INACTIVITY)
		{
			vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'EVENT_INACTIVITY', '');");
		}
	}

	mySocket.stopEvent();
}

void SocketThread::send(string data)
{
	mySocket.sendData(data);
}

