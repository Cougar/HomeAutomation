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
	SyslogStream &slog = SyslogStream::getInstance();

	//PKT 00060102 1 0 00 00 c4

	rawHex = trim(rawHex, '\n');
	rawHex = trim(rawHex);

	vector<string> parts = explode(" ", rawHex);

	if (parts.size() < 2 || parts[0].compare("PKT") != 0)
	{
		slog << "Malformed can message received from canDaemon: \"" << rawHex << "\"\n";
		throw new CanMessageException("Malformed can message received from canDaemon");
	}

	myRawHeaderBin = hex2bin(parts[1]);

	myIsHeartbeat = (parts[1] == translator.getDefineId("HeartbeatHeader", "None"));

	// This is not a module id header, but heartbeats are sent this way so we want to check it anyway
	if (myIsHeartbeat)
	{
		string rawHexData = "";
		for (int n = 4; n < parts.size(); n++)
		{
			rawHexData += parts[n];
		}

		myData = translator.translateHeartbeatData(rawHexData);
	}
	else
	{
		myClassName = translator.lookupClassName(bin2uint(myRawHeaderBin.substr(3, 4)));

		if (myClassName == "")
		{
			myIsUnknown = true;
			return;
		}

		myDirectionFlag = translator.lookupDirectionFlag(bin2uint(myRawHeaderBin.substr(7, 1)));
		myModuleName = translator.lookupModuleName(bin2uint(myRawHeaderBin.substr(8, 8)));

		myModuleId = (unsigned int)bin2uint(myRawHeaderBin.substr(16, 8));

		int commandId = bin2uint(myRawHeaderBin.substr(24, 8));
		myCommandName = translator.lookupCommandName(commandId, translator.lookupModuleName(myModuleId));

		string rawHexData = "";
		for (int n = 4; n < parts.size(); n++)
		{
			rawHexData += parts[n];
		}
		
		myData = translator.translateData(commandId, myModuleName, rawHexData);
	}
}

string CanMessage::getRaw()
{
	CanIdTranslator &translator = CanIdTranslator::getInstance();

	string rawHex = "PKT ";

	string bin = "000";

	int classId = translator.resolveClassId(myClassName);
	if (classId == -1)
		bin += "0000";
	else
		bin += uint2bin(classId, 4);


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


	
	rawHex += bin2hex(bin);
	
	rawHex += " 1 0";

	string dataHex = translator.translateDataToHex(commandNameId, myModuleName, myData);

	while (dataHex.size() > 0)
	{
		string hex = dataHex.substr(0, 2);
		dataHex.erase(0, 2);
		rawHex += " " + hex;
	}

	rawHex += "\n";

	return rawHex;
}

void CanMessage::setJSON(string json)
{
	cout << json << "\n";
	string data = trim(json, '(');
	data = trim(data, ')');
	data = trim(data, '{');
	data = trim(data, '{');
	data = trim(data, ' ');

	vector<string> parts = explode(",", data);

	for (int n = 0; n < parts.size(); n++)
	{
		string part = trim(parts[n], ' ');
		vector<string> innerParts = explode(":", part);

		int c = 0;
		if (innerParts[0] == "data")
			c++;

		string name = trim(innerParts[c], ' ');
		name = trim(name, '{');
		name = trim(name, '}');
		string value = trim(innerParts[c+1], ' ');
		value = trim(value, '{');
		value = trim(value, '}');
		value = trim(value, '"');
		
		cout << "\n" << name << "=" << value << "\n";

		if (name == "className")
			myClassName = value;
		else if (name == "directionFlag")
			myDirectionFlag = value;
		else if (name == "moduleName")
			myModuleName = value;
		else if (name == "moduleId")
			myModuleId = stoi(value);
		else if (name == "commandName")
			myCommandName = value;
		else
		{
			CanVariable canVariable(name, value, "", "");
			myData[name] = canVariable;
		}
	}
}

string CanMessage::getJSON()
{
	string json = "{ ";

	json += "className : '" + myClassName + "', ";
	json += "directionFlag : '" + myDirectionFlag + "', ";
	json += "moduleName : '" + myModuleName + "', ";
	json += "moduleId : " + itos(myModuleId) + ", ";
	json += "commandName : '" + myCommandName + "', ";

	json += "data : {";

	map<string, CanVariable>::iterator iter;

	for (iter = myData.begin(); iter != myData.end(); iter++)
	{
		json += " " + iter->first + " : { ";

		json += "unit : '" + iter->second.getUnit() + "', ";
		json += "type : '" + iter->second.getType() + "', ";
		json += "value : ";

		if (iter->second.getType() == "uint")
		{
			json += iter->second.getValue();
		}
		else if (iter->second.getType() == "float")
		{
			json += iter->second.getValue();
		}
		else if (iter->second.getType() == "ascii")
		{
			json += "'" + iter->second.getValue() + "'";
		}

		json += " },";
	}

	json = trim(json, ',');

	json += " }";

	return json + " }";
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
		else if (iter->second.getType() == "ascii")
		{
			json += "'" + escape(iter->second.getValue()) + "'";
		}

		json += ",";
	}

	json = trim(json, ',');

	return json + " }";
}
