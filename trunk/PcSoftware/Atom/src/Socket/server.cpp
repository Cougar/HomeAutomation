/***************************************************************************
 *   Copyright (C) January 4, 2009 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   server.cpp                                            *
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

#include "server.h"

Server::Server(int port, string name)
{
	setSettings(port, name);
}

Server::~Server()
{
	stop();

	for (map<int, AsyncSocket*>::iterator iter = myClientSockets.begin(); iter != myClientSockets.end(); iter++)
	{
		if (iter->second->isConnected())
		{
			iter->second->stop();
		}

		delete iter->second;
	}
}

void Server::setSettings(int port, string name)
{
	myName = name;
	mySocket.setPort(port);
}

void Server::run()
{
	Logger &log = Logger::getInstance();

	try
	{
		mySocket.startListen();

		log.add(myName + " is listening for connections on port " + itos(mySocket.getPort()) + ".\n");

		while (true)
		{
			AsyncSocket *newSocket = new AsyncSocket();

			if (mySocket.accept(newSocket))
			{
				newSocket->sendData("Welcome to Atom " + ftos(VERSION) + " - " + myName + "\r\n");

				log.add(myName + " accepted new client with id " + itos(newSocket->getId()) + ".\n");
				myClientSockets[newSocket->getId()] = newSocket;
				myClientSockets[newSocket->getId()]->eventSetCallback(this);
				myClientSockets[newSocket->getId()]->start();
			}
		}
	}
	catch (SocketException* e)
	{
		log.add(myName + " caught an exception:" + e->getDescription() + ".\n");
	}
}

void Server::sendToAll(string data)
{
	Logger &log = Logger::getInstance();

	for (map<int, AsyncSocket*>::iterator iter = myClientSockets.begin(); iter != myClientSockets.end(); iter++)
	{
		if (iter->second->isConnected())
		{
			//log.add("Server: sent data to client " + itos(iter->second->getSocket()) + ".\n";

			try
			{
				iter->second->sendData(data);
			}
			catch (SocketException* e)
			{
				log.add(myName + " caught an exception:" + e->getDescription() + ".\n");
			}
		}
		else
		{
			log.add(myName + " had a client disconnect.\n");
			delete iter->second;
			myClientSockets.erase(iter);
		}
	}
}

void Server::sendTo(int id, string data)
{
	Logger &log = Logger::getInstance();

	if (myClientSockets.find(id) != myClientSockets.end())
	{
		if (myClientSockets[id]->isConnected())
		{
			//log.add("Server: sent data to client " + itos(iter->second->getSocket()) + ".\n";

			try
			{
				myClientSockets[id]->sendData(data);
			}
			catch (SocketException* e)
			{
				log.add(myName + " caught an exception:" + e->getDescription() + ".\n");
			}
		}
		else
		{
			log.add(myName + " had a client disconnect.\n");
			
			delete myClientSockets[id];
			myClientSockets[id]=0;
			myClientSockets.erase(id);
		}
	}
}

void Server::disconnectClient(int id)
{
	Logger &log = Logger::getInstance();

	if (myClientSockets.find(id) != myClientSockets.end())
	{
//		log.add(myName + " had a client disconnect!.\n");
		
//		delete myClientSockets[id];

//		myClientSockets.erase(id);
//delete myClientSockets[id];
//			myClientSockets[id]=0;
//			myClientSockets.erase(id);
	}
}

void Server::handleEvent(int id, SocketEvent socketEvent)
{
	switch (socketEvent.getType())
	{
		case SocketEvent::TYPE_CONNECTION_CLOSED:
		case SocketEvent::TYPE_CONNECTION_DIED:
		case SocketEvent::TYPE_CONNECTION_RESET:
		disconnectClient(id);
		break;

		case SocketEvent::TYPE_DATA:
		handleClientData(id, socketEvent.getData());
		break;
	}
}

void Server::handleClientData(int id, string data)
{
	Logger &log = Logger::getInstance();
	log.add(myName + " received data from client " + itos(id) + ": \"" + data + "\"\n");
}
