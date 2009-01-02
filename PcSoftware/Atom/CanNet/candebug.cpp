/***************************************************************************
 *   Copyright (C) December 27, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   candebug.cpp                                            *
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

#include "candebug.h"

CanDebug* CanDebug::myInstance = NULL;

CanDebug& CanDebug::getInstance()
{
	if (myInstance == NULL)
	{
		myInstance = new CanDebug();
	}

	return *myInstance;
}

void CanDebug::deleteInstance()
{
	if (myInstance != NULL)
	{
		delete myInstance;
		myInstance = NULL;
	}
}

CanDebug::CanDebug()
{
	Thread<CanDebug>();
}

CanDebug::~CanDebug()
{
	stop();

	map<int, AsyncSocket*>::iterator iter;

	for (iter = myClientSockets.begin(); iter != myClientSockets.end(); iter++)
	{
		if (iter->second->isConnected())
		{
			iter->second->stop();
		}

		delete iter->second;
	}
}

void CanDebug::run()
{
	SyslogStream &slog = SyslogStream::getInstance();

	string port = Settings::get("CanDebugPort");

	if (port == "")
	{
		slog << "CanDebugPort is not defined in the config file, will not start debug interface\n";
		return;
	}

	try
	{
		mySocket.setPort(stoi(port));

		mySocket.startListen();

		slog << "CanDebug is listening for connections on port " + port + ".\n";

		while (true)
		{
			AsyncSocket *newSocket = new AsyncSocket();

			if (mySocket.accept(newSocket))
			{
				newSocket->sendData("Welcome to Atom " + ftos(VERSION) + " - CanDebug output\n");

				slog << "CanDebug accepted new client on socket " + itos(newSocket->getSocket()) + ".\n";
				myClientSockets[newSocket->getSocket()] = newSocket;
				myClientSockets[newSocket->getSocket()]->start();
			}
		}
	}
	catch (SocketException* e)
	{
		slog << "Exception was caught:" + e->getDescription() + ".\n";
	}
}

void CanDebug::sendData(string data)
{
	SyslogStream &slog = SyslogStream::getInstance();

	map<int, AsyncSocket*>::iterator iter;

	for (iter = myClientSockets.begin(); iter != myClientSockets.end(); iter++)
	{
		if (iter->second->isConnected())
		{
			//slog << "CanDebug: sent data to client " + itos(iter->second->getSocket()) + ".\n";
			
			try
			{
				iter->second->sendData(data);
			}
			catch (SocketException* e)
			{
				slog << "CanDebug caught an exception:" + e->getDescription() + ".\n";
			}
		}
		else
		{
			slog << "CanDebug had a client disconnect.\n";
			delete iter->second;
			myClientSockets.erase(iter);
		}
	}
}

void CanDebug::sendCanMessage(CanMessage canMessage)
{
	if (myClientSockets.size() > 0)
	{
		string debugData = "";

		if (canMessage.getClassName() == "nmt")
		{
			debugData += "NMT";
		}
		else
		{
			if (canMessage.getDirectionFlag() == "To_Owner")
			{
				debugData += "RX";
			}
			else if (canMessage.getDirectionFlag() == "From_Owner")
			{
				debugData += "TX";
			}
			else
			{
				debugData += "??";
			}

			debugData += " " + canMessage.getClassName();
			debugData += "_" + canMessage.getModuleName();
			debugData += ":" + itos(canMessage.getModuleId());
		}

		debugData += " CMD=" + canMessage.getCommandName() + " ";

		map<string, CanVariable> vars = canMessage.getData();

		map<string, CanVariable>::iterator iter;

		for (iter = vars.begin(); iter != vars.end(); iter++)
		{
			debugData += " " + iter->second.getName() + "=" + iter->second.getValue();
		}

		sendData("[" + niceTime() + "] " + debugData + "\n");
	}
}
