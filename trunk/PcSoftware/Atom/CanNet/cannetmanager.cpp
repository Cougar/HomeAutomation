/***************************************************************************
 *   Copyright (C) November 29, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   cannetmanager.cpp                                            *
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

#include "canmessage.h"
#include "cannetmanager.h"

CanNetManager* CanNetManager::myInstance = NULL;

CanNetManager& CanNetManager::getInstance()
{
	if (myInstance == NULL)
	{
		myInstance = new CanNetManager();
	}

	return *myInstance;
}

void CanNetManager::deleteInstance()
{
	if (myInstance != NULL)
	{
		delete myInstance;
		myInstance = NULL;
	}
}

CanNetManager::CanNetManager()
{
	CanIdTranslator &translator = CanIdTranslator::getInstance();
	myChannel = new AsyncSocket();
}

CanNetManager::~CanNetManager()
{
	stop();
	CanIdTranslator::deleteInstance();
	myChannel->stop();
	delete myChannel;
}

void CanNetManager::run()
{
	SyslogStream &slog = SyslogStream::getInstance();
	VirtualMachine &vm = VirtualMachine::getInstance();
	CanDebug &canDebug = CanDebug::getInstance();

	string address = Settings::get("CanNetAddress");
	if (address == "")
	{
		throw new Atom::Exception("CanNetAddress is not defined in the config file, can not start.");
	}

	string port = Settings::get("CanNetPort");
	if (port == "")
	{
		throw new Atom::Exception("CanNetPort is not defined in the config file, can not start.");
	}

	myChannel->setAddress(address);
	myChannel->setPort(stoi(port));
	myChannel->setReconnectTimeout(10);

	myChannel->start();

	myChannel->eventStartListen();

	bool waitingForPong = false;
	vector<string> dataLines;
	string data;
	CanMessage *canMessage = NULL;
	string expression;

	while (1)
	{
		myChannel->eventWait();

		while (myChannel->eventIsAvailable())
		{
			SocketEvent socketEvent = myChannel->eventFetch();

			switch (socketEvent.getType())
			{
				case SocketEvent::TYPE_CONNECTED:
				slog << "Connected to " + myChannel->getAddress() + ":" + itos(myChannel->getPort()) + ".\n";
				break;

				case SocketEvent::TYPE_CONNECTING:
				slog << "Connecting to " + myChannel->getAddress() + ":" + itos(myChannel->getPort()) + ".\n";
				break;

				case SocketEvent::TYPE_CONNECTION_CLOSED:
				slog << "Connection to " + myChannel->getAddress() + ":" + itos(myChannel->getPort()) + " closed.\n";
				vm.queueExpression("setAllOffline();");
				break;

				case SocketEvent::TYPE_CONNECTION_DIED:
				slog << "Connection to " + myChannel->getAddress() + ":" + itos(myChannel->getPort()) + " died :: " + socketEvent.getData() + "\n";
				vm.queueExpression("setAllOffline();");
				break;

				case SocketEvent::TYPE_CONNECTION_RESET:
				slog << "Connection to " + myChannel->getAddress() + ":" + itos(myChannel->getPort()) + " was reset :: " + socketEvent.getData() + "\n";
				vm.queueExpression("setAllOffline();");
				break;

				case SocketEvent::TYPE_CONNECTION_FAILED:
				slog << "Connection to " + myChannel->getAddress() + ":" + itos(myChannel->getPort()) + " failed :: " + socketEvent.getData() + "\n";
				vm.queueExpression("setAllOffline();");
				break;

				case SocketEvent::TYPE_DATA:
				waitingForPong = false;
				try
				{
					dataLines = explode("\n", socketEvent.getData());

					for (int n = 0; n < dataLines.size(); n++)
					{
						data = trim(dataLines[n], '\n');

						if (data == "PONG")
						{
							slog << "Received pong.\n";
							continue;
						}

						canMessage = new CanMessage(data);

						if (!canMessage->isUnknown())
						{
							if (canMessage->getClassName() == "nmt")
							{
								expression = "handleNMTMessage(";
								expression += "'" + canMessage->getClassName() + "', ";
								expression += "'" + canMessage->getCommandName() + "', ";
								expression += canMessage->getJSONData();
								expression += ");";
							}
							else
							{
								expression = "handleMessage(";
								expression += "'" + canMessage->getClassName() + "', ";
								expression += "'" + canMessage->getDirectionFlag() + "', ";
								expression += "'" + canMessage->getModuleName() + "', ";
								expression += "" + itos(canMessage->getModuleId()) + ", ";
								expression += "'" + canMessage->getCommandName() + "', ";
								expression += canMessage->getJSONData();
								expression += ");";
							}

							vm.queueExpression(expression);
							canDebug.sendCanMessageToAll(*canMessage);
						}
						else
						{
							canDebug.sendToAll(data + "\n");
						}

						delete canMessage;
						canMessage = NULL;
					}
				}
				catch (CanMessageException* e)
				{
					if (canMessage != NULL)
					{
						delete canMessage;
					}

					slog << "CanMessageException was caught:\n";
					slog << e->getDescription() + "\n";
				}
				break;

				case SocketEvent::TYPE_INACTIVITY:
				if (waitingForPong)
				{
					slog << "We have not received a pong for our ping.\n";
					waitingForPong = false;
					vm.queueExpression("setAllOffline();");
					myChannel->forceReconnect();
				}
				else
				{
					//slog << "We have not received anything from the canDaemon in some time.\n";
					waitingForPong = true;
					slog << "Sending ping.\n";
					myChannel->sendData("PING");
				}
				break;

				case SocketEvent::TYPE_WAITING_RECONNECT:
				slog << "Will try to reconnect to " + myChannel->getAddress() + ":" + itos(myChannel->getPort()) + " in " + itos(myChannel->getReconnectTimeout()) + " seconds.\n";
				break;
			}
		}
	}

	myChannel->eventStopListen();
}

void CanNetManager::sendMessage(CanMessage canMessage)
{
	CanDebug &canDebug = CanDebug::getInstance();
	myChannel->sendData(canMessage.getRaw());
	canDebug.sendCanMessageToAll(canMessage);
}

