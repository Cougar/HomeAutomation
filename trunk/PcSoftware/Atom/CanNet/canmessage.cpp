/***************************************************************************
 *   Copyright (C) November 28, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   canmessage.cpp                                            *
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

#include <string>
#include <iostream>
#include "canidtranslator.h"
#include "canmessage.h"

void CanMessage::setRaw(string rawHex)
{
	CanIdTranslator &translator = CanIdTranslator::getInstance();
	//SyslogStream &slog = SyslogStream::getInstance();

	//PKT 00060102 1 0 00 00 c4

	rawHex = trim(rawHex, '\n');
	rawHex = trim(rawHex);

	vector<string> parts = explode(" ", rawHex);

	if (parts.size() < 2 || parts[0].compare("PKT") != 0)
	{
		//slog << "Malformed can message received from canDaemon: \"" << rawHex << "\"\n";
		throw new CanMessageException("Malformed can message received from canDaemon");
	}

	string rawHeaderBin = hex2bin(parts[1]);

	myClassName = translator.lookupClassName(bin2uint(rawHeaderBin.substr(3, 4)));

	if (myClassName == "")
	{
		myIsUnknown = true;
		return;
	}

	string rawHexData = "";
	for (int n = 4; n < parts.size(); n++)
	{
		rawHexData += parts[n];
	}

	if (myClassName == "nmt")
	{
		int commandId = bin2uint(rawHeaderBin.substr(8, 8));
		myCommandName = translator.lookupNMTCommandName(commandId);
		
		myData = translator.translateNMTData(commandId, rawHexData);
	}
	else
	{
		myDirectionFlag = translator.lookupDirectionFlag(bin2uint(rawHeaderBin.substr(7, 1)));
		myModuleName = translator.lookupModuleName(bin2uint(rawHeaderBin.substr(8, 8)));

		myModuleId = bin2uint(rawHeaderBin.substr(16, 8));

		int commandId = bin2uint(rawHeaderBin.substr(24, 8));
		myCommandName = translator.lookupCommandName(commandId, myModuleName);

		myData = translator.translateData(commandId, myModuleName, rawHexData);
	}
}

string CanMessage::getRaw()
{
	CanIdTranslator &translator = CanIdTranslator::getInstance();

	string dataHex;

	string rawHex = "PKT ";

	string bin = "000";

	int classId = translator.resolveClassId(myClassName);
	if (classId == -1)
		bin += "0000";
	else
		bin += uint2bin(classId, 4);

	if (myClassName == "nmt")
	{
		bin +="0";

		int commandNameId = translator.resolveNMTCommandId(myCommandName);
		if (commandNameId == -1)
			bin += "00000000";
		else
			bin += uint2bin(commandNameId, 8);

		bin += "0000000000000000";

		dataHex = translator.translateNMTDataToHex(commandNameId, myData);
	}
	else
	{
		bin += uint2bin(translator.resolveDirectionFlag(myDirectionFlag), 1);


		int moduleTypeId = translator.resolveModuleId(myModuleName);
		if (moduleTypeId == -1)
			bin += "00000000";
		else
			bin += uint2bin(moduleTypeId, 8);


		int moduleId = myModuleId;
		bin += uint2bin(moduleId, 8);


		int commandNameId = translator.resolveCommandId(myCommandName, myModuleName);
		if (commandNameId == -1)
			bin += "00000000";
		else
			bin += uint2bin(commandNameId, 8);

		dataHex = translator.translateDataToHex(commandNameId, myModuleName, myData);
	}

	rawHex += bin2hex(bin);

	rawHex += " 1 0";

	while (dataHex.size() > 0)
	{
		string hex = dataHex.substr(0, 2);
		dataHex.erase(0, 2);
		rawHex += " " + hex;
	}

	rawHex += "\n";

	return rawHex;
}

void CanMessage::setData(map<string, CanVariable> data)
{
	myData = data;

	CanIdTranslator &translator = CanIdTranslator::getInstance();

	int commandNameId = translator.resolveNMTCommandId(myCommandName);

	if (myClassName == "nmt")
	{
		translator.makeNMTDataValid(commandNameId, data);
	}
	else
	{
		translator.makeDataValid(commandNameId, myModuleName, data);
	}
}

string CanMessage::getJSONData()
{
	string json = "{";

	map<string, CanVariable>::iterator iter;

	for (iter = myData.begin(); iter != myData.end(); iter++)
	{
		json += " " + iter->first + " : ";

		if (iter->second.getType() == "uint")
		{
			json += iter->second.getValue();
		}
		else if (iter->second.getType() == "float")
		{
			json += iter->second.getValue();
		}
		else if (iter->second.getType() == "ascii" || iter->second.getType() == "hexstring")
		{
			json += "'" + escape(iter->second.getValue()) + "'";
		}

		json += ",";
	}

	json = trim(json, ',');

	return json + " }";
}
