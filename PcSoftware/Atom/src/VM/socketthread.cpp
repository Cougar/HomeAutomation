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

SocketThread::SocketThread(string address, unsigned int port, unsigned int reconnectTimeout)
{
	myId = time(NULL);

	mySocket = new AsyncSocket();

	mySocket->setAddress(address);
	mySocket->setPort(port);
	mySocket->setReconnectTimeout(reconnectTimeout);
}

SocketThread::~SocketThread()
{
	if (!stop())
	{
		cout << "~SocketThread() :: Failed to stop thread :: Error code is " + itos(getError()) + "\n";

	}
/*	else
	{
		cout << "~SocketThread() :: Successfully stopped thread\n";
	}*/
	delete mySocket;
}

void SocketThread::run()
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	mySocket->start();

	mySocket->eventStartListen();

	while (true)
	{
		mySocket->eventWait();

		while (mySocket->eventIsAvailable())
		{
			SocketEvent socketEvent = mySocket->eventFetch();

			switch (socketEvent.getType())
			{
				case SocketEvent::TYPE_CONNECTED:
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'CONNECTED', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_CONNECTING:
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'CONNECTING', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_CONNECTION_CLOSED:
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'CLOSED', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_CONNECTION_DIED:
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'DIED', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_CONNECTION_RESET:
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'RESET', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_CONNECTION_FAILED:
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'CONNECTION_FAILED', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_DATA:
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'DATA', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_INACTIVITY:
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'INACTIVITY', '" + escape(socketEvent.getData()) + "');");
				break;
				
				case SocketEvent::TYPE_WAITING_RECONNECT:
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'WAITING_RECONNECT', '" + escape(socketEvent.getData()) + "');");
				break;
			}
		}
	}

	mySocket->eventStopListen();
}

void SocketThread::send(string data)
{
	mySocket->sendData(data);
}

