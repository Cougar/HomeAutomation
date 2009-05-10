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

unsigned long int SocketThread::myCount = 0;
Mutex SocketThread::myCountMutex;

SocketThread::SocketThread(string address, unsigned int port, unsigned int reconnectTimeout)
{
	myCountMutex.lock();
	myId = myCount++;
	myCountMutex.unlock();

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
	else
	{
		//cout << "~SocketThread() :: Successfully stopped thread " << myId << "\n";
	}
	delete mySocket;
}

void SocketThread::startSocket()
{
cout << "SocketThread::run - starting socket \n";
	mySocket->start();
}

void SocketThread::run()
{
//cout << "SocketThread::run - Thread started " << myId << "\n";
	VirtualMachine &vm = VirtualMachine::getInstance();

//cout << "SocketThread::run - starting socket \n";
//	mySocket->start();

//cout << "SocketThread::run - locking mutex \n";
	mySocket->eventStartListen();

cout << "SocketThread::run - start wait \n";
	while (true)
	{
		mySocket->eventWait();
//cout << "SocketThread::run - eventWait over \n";

		while (mySocket->eventIsAvailable())
		{
			SocketEvent socketEvent = mySocket->eventFetch();

			switch (socketEvent.getType())
			{
				case SocketEvent::TYPE_CONNECTED:
//cout << "SocketThread::run - event CONNECTED \n";
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'CONNECTED', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_CONNECTING:
//cout << "SocketThread::run - event CONNECTING \n";
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'CONNECTING', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_CONNECTION_CLOSED:
//cout << "SocketThread::run - event CLOSED \n";
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'CLOSED', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_CONNECTION_DIED:
//cout << "SocketThread::run - event DIED \n";
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'DIED', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_CONNECTION_RESET:
//cout << "SocketThread::run - event RESET \n";
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'RESET', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_CONNECTION_FAILED:
//cout << "SocketThread::run - event CONNECTION_FAILED \n";
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'CONNECTION_FAILED', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_DATA:
//cout << "SocketThread::run - event DATA \n";
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'DATA', '" + escape(socketEvent.getData()) + "');");
				break;

				case SocketEvent::TYPE_INACTIVITY:
//cout << "SocketThread::run - event INACTIVITY \n";
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'INACTIVITY', '" + escape(socketEvent.getData()) + "');");
				break;
				
				case SocketEvent::TYPE_WAITING_RECONNECT:
//cout << "SocketThread::run - event WAITING_RECONNECT \n";
				vm.queueExpression("Socket.triggerSocketCallback(" + itos(myId) + ", 'WAITING_RECONNECT', '" + escape(socketEvent.getData()) + "');");
				break;
			}
		}
	}

//cout << "SocketThread::run - Thread end?\n";
	mySocket->eventStopListen();
}

void SocketThread::send(string data)
{
//cout << "SocketThread::send - Sending data\n";
	mySocket->sendData(data);
}

