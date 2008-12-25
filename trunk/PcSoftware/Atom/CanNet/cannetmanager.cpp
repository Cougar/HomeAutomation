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
	CanIdTranslator::deleteInstance();
	delete myChannel;
}

void CanNetManager::openChannel()
{
	SyslogStream &slog = SyslogStream::getInstance();
	VirtualMachine &vm = VirtualMachine::getInstance();

	string address = Settings::get("CanNetAddress");
	string port = Settings::get("CanNetPort");

	myChannel->setAddress(address, stoi(port));

	myChannel->start();

	myChannel->startEvent();

	while (1)
	{
		myChannel->waitForEvent();

		int event = myChannel->getEvent();

		if (event == ASYNCSOCKET_EVENT_DATA)
		{
			while (myChannel->availableData())
			{
				string data = myChannel->getData();

				try
				{
					vector<string> dataLines = explode("\n", data);

					for (int n = 0; n < dataLines.size(); n++)
					{
						CanMessage canMessage;
						canMessage.setRaw(data);

						//slog << "Received: " << data;

						if (!canMessage.isUnknown())
						{
							vm.queueCanMessage(canMessage);
						}
						/*else
						{
							slog << "Received unknown message. Skipping...\n";
						}*/
					}
				}
				catch (CanMessageException* e)
				{
					slog << "CanMessageException was caught:\n";
					slog << e->getDescription() << "\n";
				}
			}
		}
	}

	myChannel->stopEvent();
}

void CanNetManager::sendMessage(CanMessage canMessage)
{
	myChannel->sendData(canMessage.getRaw());
}

