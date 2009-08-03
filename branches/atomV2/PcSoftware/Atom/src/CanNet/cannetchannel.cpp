/***************************************************************************
 *   Copyright (C) November 27, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   cannetchannel.cpp                                            *
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

#include "cannetmanager.h"


#include "cannetchannel.h"

CanNetChannel::CanNetChannel()
{
}

CanNetChannel::~CanNetChannel()
{
}

void CanNetChannel::sendString(string data)
{
	SyslogStream &slog = SyslogStream::getInstance();

	try
	{
		myClientSocket << data;
	}
	catch (SocketException& e)
	{
		slog << "SocketException was caught:\n";
		slog << e.getDescription() << "\n";
	}
}

void CanNetChannel::run()
{
	CanNetManager &canMan = CanNetManager::getInstance();
	SyslogStream &slog = SyslogStream::getInstance();

	try
	{
		string address = Settings::get("CanNetAddress");
		string port = Settings::get("CanNetPort");

		slog << "Trying to connect to canDaemon on " << address << ":" << port << "...\n";

		myClientSocket.start(address, atoi(port.c_str()));

		slog << "Connected to canDaemon.\n";

		string data;

		while (1)
		{
			myClientSocket >> data;

			try
			{
				CanMessage canMessage;
				canMessage.setRaw(data);

				if (canMessage.isUnknown())
				{
					slog << "Received unknown message. Skipping...\n";
				}
				else
				{
					canMan.addToInQueue(canMessage);
				}
			}
			catch (CanMessageException& e)
			{
				slog << "CanMessageException was caught:\n";
				slog << e.getDescription() << "\n";
			}

			//myClientSocket << "TEST\n";

			data = "";
		}
	}
	catch (SocketException& e)
	{
		slog << "SocketException was caught:\n";
		slog << e.getDescription() << "\n";
	}
}

