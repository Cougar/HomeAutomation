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
}

CanDebug::~CanDebug()
{
}

void CanDebug::sendCanMessageToAll(CanMessage canMessage)
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
		map<int, string> sortMap;

		for (map<string, CanVariable>::iterator iter = vars.begin(); iter != vars.end(); iter++)
		{
			sortMap[iter->second.getStartBit()] = " " + iter->second.getName() + "=" + iter->second.getValue();
		}

		for (map<int, string>::iterator iter = sortMap.begin(); iter != sortMap.end(); iter++)
		{
			debugData += iter->second;
		}

		sendToAll("[" + niceTime() + "] " + debugData + "\r\n");
	}
}

void CanDebug::handleClientData(int id, string data)
{

}
