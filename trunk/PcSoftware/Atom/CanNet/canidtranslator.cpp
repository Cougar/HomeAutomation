/***************************************************************************
 *   Copyright (C) November 28, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   canidtranslator.cpp                                            *
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

#include "canidtranslator.h"

CanIdTranslator* CanIdTranslator::myInstance = NULL;

CanIdTranslator& CanIdTranslator::getInstance()
{
	if (myInstance == NULL)
	{
		myInstance = new CanIdTranslator();
	}

	return *myInstance;
}

void CanIdTranslator::deleteInstance()
{
	delete myInstance;
	myInstance = NULL;
}

CanIdTranslator::CanIdTranslator()
{
	SyslogStream &slog = SyslogStream::getInstance();

	string filename = Settings::get("CanIdXMLFile");

	if (file_exists(filename))
	{
		xmlNode.load(filename.c_str());

		slog << "Loading definitions from " + filename + ".\n";
	}
	else
	{
		slog << "CanIdXMLFile is not defined in the config file, this will probably not work at all.\n";
	}
}

string CanIdTranslator::lookupClassName(int classId)
{
	XmlNode classesNode = xmlNode.findChild("classes");
	vector<XmlNode> classNodes = classesNode.getChildren();

	for (int n = 0; n < classNodes.size(); n++)
	{
		map<string, string> attributes = classNodes[n].getAttributes();

		if (stoi(attributes["id"]) == classId)
		{
			return attributes["name"];
		}
	}

	return "";
}

int CanIdTranslator::resolveClassId(string className)
{
	XmlNode classesNode = xmlNode.findChild("classes");
	vector<XmlNode> classNodes = classesNode.getChildren();

	for (int n = 0; n < classNodes.size(); n++)
	{
		map<string, string> attributes = classNodes[n].getAttributes();

		if (attributes["name"].compare(className) == 0)
		{
			return stoi(attributes["id"]);
		}
	}

	return -1;
}

string CanIdTranslator::lookupCommandName(int commandId, string moduleName)
{
	XmlNode commandsNode = xmlNode.findChild("commands");
	vector<XmlNode> commandNodes = commandsNode.getChildren();

	for (int n = 0; n < commandNodes.size(); n++)
	{
		map<string, string> attributes = commandNodes[n].getAttributes();

		if (commandId >= 128)
		{
			if (stoi(attributes["id"]) == commandId && attributes["module"] == moduleName)
			{
				return attributes["name"];
			}
		}
		else if (stoi(attributes["id"]) == commandId)
		{
			return attributes["name"];
		}
	}

	return "";
}

int CanIdTranslator::resolveCommandId(string commandName, string moduleName)
{
	XmlNode commandsNode = xmlNode.findChild("commands");
	vector<XmlNode> commandNodes = commandsNode.getChildren();

	for (int n = 0; n < commandNodes.size(); n++)
	{
		map<string, string> attributes = commandNodes[n].getAttributes();

		if (attributes["name"].compare(commandName) == 0)
		{
			int commandId = stoi(attributes["id"]);

			if (commandId >= 128)
			{
				if (attributes["module"] == moduleName)
				{
					return commandId;
				}
			}
			else
			{
				return commandId;
			}
		}
	}

	return -1;
}

string CanIdTranslator::lookupNMTCommandName(int commandId)
{
	XmlNode commandsNode = xmlNode.findChild("nmt_messages");
	vector<XmlNode> commandNodes = commandsNode.getChildren();

	for (int n = 0; n < commandNodes.size(); n++)
	{
		map<string, string> attributes = commandNodes[n].getAttributes();

		if (stoi(attributes["id"]) == commandId)
		{
			return attributes["name"];
		}
	}

	return "";
}

int CanIdTranslator::resolveNMTCommandId(string commandName)
{
	XmlNode commandsNode = xmlNode.findChild("nmt_messages");
	vector<XmlNode> commandNodes = commandsNode.getChildren();

	for (int n = 0; n < commandNodes.size(); n++)
	{
		map<string, string> attributes = commandNodes[n].getAttributes();

		if (attributes["name"].compare(commandName) == 0)
		{
			return stoi(attributes["id"]);
		}
	}

	return -1;
}

string CanIdTranslator::getDefineId(string name, string group)
{
	XmlNode definesNode = xmlNode.findChild("defines");
	vector<XmlNode> defineNodes = definesNode.getChildren();

	for (int n = 0; n < defineNodes.size(); n++)
	{
		map<string, string> attributes = defineNodes[n].getAttributes();

		if (attributes["group"] == group && attributes["name"] == name)
		{
			return attributes["id"];
		}
	}

	return "";
}

string CanIdTranslator::lookupDirectionFlag(int directionFlag)
{
	XmlNode definesNode = xmlNode.findChild("defines");
	vector<XmlNode> defineNodes = definesNode.getChildren();

	for (int n = 0; n < defineNodes.size(); n++)
	{
		map<string, string> attributes = defineNodes[n].getAttributes();

		if (attributes["group"].compare("DirectionFlag") == 0 && stoi(attributes["id"]) == directionFlag)
		{
			return attributes["name"];
		}
	}

	return "";
}

int CanIdTranslator::resolveDirectionFlag(string directionName)
{
	XmlNode definesNode = xmlNode.findChild("defines");
	vector<XmlNode> defineNodes = definesNode.getChildren();

	for (int n = 0; n < defineNodes.size(); n++)
	{
		map<string, string> attributes = defineNodes[n].getAttributes();

		if (attributes["group"].compare("DirectionFlag") == 0 && attributes["name"].compare(directionName) == 0)
		{
			return stoi(attributes["id"]);
		}
	}

	return -1;
}

string CanIdTranslator::lookupModuleName(int moduleId)
{
	XmlNode modulesNode = xmlNode.findChild("modules");
	vector<XmlNode> moduleNodes = modulesNode.getChildren();

	for (int n = 0; n < moduleNodes.size(); n++)
	{
		map<string, string> attributes = moduleNodes[n].getAttributes();

		if (stoi(attributes["id"]) == moduleId)
		{
			return attributes["name"];
		}
	}

	return "";
}

int CanIdTranslator::resolveModuleId(string moduleName)
{
	XmlNode modulesNode = xmlNode.findChild("modules");
	vector<XmlNode> moduleNodes = modulesNode.getChildren();

	for (int n = 0; n < moduleNodes.size(); n++)
	{
		map<string, string> attributes = moduleNodes[n].getAttributes();

		if (attributes["name"].compare(moduleName) == 0)
		{
			return stoi(attributes["id"]);
		}
	}

	return -1;
}

string CanIdTranslator::translateDataToHex(int commandId, string moduleName, map<string, CanVariable> data)
{
	XmlNode commandsNode = xmlNode.findChild("commands");
	vector<XmlNode> commandNodes = commandsNode.getChildren();

	map<string, CanVariable> sortedData;

	for (int n = 0; n < commandNodes.size(); n++)
	{
		map<string, string> attributes = commandNodes[n].getAttributes();

		bool correct = false;

		if (commandId >= 128)
		{
			if (stoi(attributes["id"]) == commandId && attributes["module"] == moduleName)
			{
				correct = true;
			}
		}
		else if (stoi(attributes["id"]) == commandId)
		{
			correct = true;
		}

		if (correct)
		{
			XmlNode varablesNode = commandNodes[n].findChild("variables");

			vector<XmlNode> variableNodes = varablesNode.getChildren();

			for (int c = 0; c < variableNodes.size(); c++)
			{
				map<string, string> attributes = variableNodes[c].getAttributes();

				if (data.find(attributes["name"]) != data.end())
				{
					int bitStart = stoi(attributes["start_bit"]);///FIXME: This is not a good way
					int bitLength = stoi(attributes["bit_length"]);

					sortedData[attributes["name"]].setType(attributes["type"]);
					sortedData[attributes["name"]].setUnit(attributes["unit"]);
					sortedData[attributes["name"]].setBitLength(bitLength);
					sortedData[attributes["name"]].setBitLength(bitLength);
					sortedData[attributes["name"]].setStartBit(bitStart);
					sortedData[attributes["name"]].setName(attributes["name"]);
					sortedData[attributes["name"]].setValue(data[attributes["name"]].getValue());
				}
			}
		}
	}

	string bin = "0000000000000000000000000000000000000000000000000000000000000000";
	int heighestBit = 0;

	map<string, CanVariable>::iterator iter;

	for (iter = sortedData.begin(); iter != sortedData.end(); iter++)///FIXME: We should check order and length
	{
		if (iter->second.getType() == "uint")
		{
			if (iter->second.getStartBit() + iter->second.getBitLength() > heighestBit)
				heighestBit = iter->second.getStartBit() + iter->second.getBitLength();

			//cout << iter->first << ":" <<  stoi(iter->second.getValue()) << endl;
			unsigned int a = stoi(iter->second.getValue());
			
			bin.replace(iter->second.getStartBit(), iter->second.getBitLength(), uint2bin(a, iter->second.getBitLength()));
		}
		else if (iter->second.getType() == "float")
		{
			if (iter->second.getStartBit() + iter->second.getBitLength() > heighestBit)
				heighestBit = iter->second.getStartBit() + iter->second.getBitLength();

			bin.replace(iter->second.getStartBit(), iter->second.getBitLength(), float2bin(stof(iter->second.getValue()), iter->second.getBitLength()));
		}
		else if (iter->second.getType() == "ascii")
		{
			//cout << iter->second.getValue() << endl;
			for (int n = 0; n < iter->second.getValue().length(); n++)
			{
				if (iter->second.getStartBit()+n*8 + 8 > heighestBit)
					heighestBit = iter->second.getStartBit()+n*8 + 8;

				//cout << iter->second.getValue()[n] << ":" << (unsigned int)iter->second.getValue()[n] << endl;
				bin.replace(iter->second.getStartBit()+n*8, 8, uint2bin((unsigned int)iter->second.getValue()[n], 8));
			}
		}
		else if (iter->second.getType() == "hexstring")
		{
			for (int n = 0; n < iter->second.getValue().length(); n++)
			{
				if (iter->second.getStartBit()+n*4 + 4 > heighestBit)
					heighestBit = iter->second.getStartBit()+n*4 + 4;

				string character;
				character += iter->second.getValue()[n];

				bin.replace(iter->second.getStartBit()+n*4, 4, hex2bin(character));
			}
		}
	}

	bin = bin.substr(0, heighestBit);

	//cout << bin2hex(bin) << endl;

	return bin2hex(bin);
}

map<string, CanVariable> CanIdTranslator::translateData(int commandId, string moduleName, string dataHex)
{
	map<string, CanVariable> variables;
	XmlNode commandsNode = xmlNode.findChild("commands");
	vector<XmlNode> commandNodes = commandsNode.getChildren();

	string dataBin = hex2bin(dataHex);

	for (int n = 0; n < commandNodes.size(); n++)
	{
		map<string, string> attributes = commandNodes[n].getAttributes();

		bool correct = false;

		if (commandId >= 128)
		{
			if (stoi(attributes["id"]) == commandId && attributes["module"] == moduleName)
			{
				correct = true;
			}
		}
		else if (stoi(attributes["id"]) == commandId)
		{
			correct = true;
		}

		if (correct)
		{
			XmlNode varablesNode = commandNodes[n].findChild("variables");

			vector<XmlNode> variableNodes = varablesNode.getChildren();

			for (int c = 0; c < variableNodes.size(); c++)
			{
				map<string, string> attributes = variableNodes[c].getAttributes();

				int bitStart = stoi(attributes["start_bit"]);
				int bitLength = stoi(attributes["bit_length"]);

				string bits = dataBin.substr(bitStart, bitLength);
				
				CanVariable variable;
				
				variable.setName(attributes["name"]);
				variable.setType(attributes["type"]);
				variable.setUnit(attributes["unit"]);
				variable.setBitLength(bitLength);

				if (attributes["type"] == "uint")
				{
					variable.setValue(itos(bin2uint(bits)));
				}
				else if (attributes["type"] == "float")
				{
					variable.setValue(ftos(bin2float(bits)));
				}
				else if (attributes["type"] == "ascii")
				{
					string str;

					for (int k = 0; k < bits.length(); k += 8)
					{
						str += (char)bin2uint(bits.substr(k, 8));
					}

					variable.setValue(str);
				}
				else if (attributes["type"] == "hexstring")
				{
					string str;

					for (int k = 0; k < bits.length(); k += 4)
					{
						str += bin2hex(bits.substr(k, 4));
					}

					variable.setValue(str);
				}

				variables[attributes["name"]] = variable;
			}

			return variables;
		}
	}

	return variables;
}

map<string, CanVariable> CanIdTranslator::translateNMTData(int commandId, string dataHex)
{
	map<string, CanVariable> variables;
	XmlNode commandsNode = xmlNode.findChild("nmt_messages");
	vector<XmlNode> commandNodes = commandsNode.getChildren();

	string dataBin = hex2bin(dataHex);

	for (int n = 0; n < commandNodes.size(); n++)
	{
		map<string, string> attributes = commandNodes[n].getAttributes();

		if (stoi(attributes["id"]) == commandId)
		{
			XmlNode varablesNode = commandNodes[n].findChild("variables");

			vector<XmlNode> variableNodes = varablesNode.getChildren();

			for (int c = 0; c < variableNodes.size(); c++)
			{
				map<string, string> attributes = variableNodes[c].getAttributes();

				int bitStart = stoi(attributes["start_bit"]);
				int bitLength = stoi(attributes["bit_length"]);

				string bits = dataBin.substr(bitStart, bitLength);

				CanVariable variable;

				variable.setName(attributes["name"]);
				variable.setType(attributes["type"]);
				variable.setUnit(attributes["unit"]);
				variable.setBitLength(bitLength);

				if (attributes["type"] == "uint")
				{
					variable.setValue(itos(bin2uint(bits)));
				}
				else if (attributes["type"] == "float")
				{
					variable.setValue(ftos(bin2float(bits)));
				}
				else if (attributes["type"] == "ascii")
				{
					string str;

					for (int k = 0; k < bits.length(); k += 8)
					{
						str += (char)bin2uint(bits.substr(k, 8));
					}

					variable.setValue(str);
				}
				else if (attributes["type"] == "hexstring")
				{
					string str;

					for (int k = 0; k < bits.length(); k += 4)
					{
						str += bin2hex(bits.substr(k, 4));
					}

					variable.setValue(str);
				}

				variables[attributes["name"]] = variable;
			}

			return variables;
		}
	}

	//return dataString + " ]";
	return variables;
}

string CanIdTranslator::translateNMTDataToHex(int commandId, map<string, CanVariable> data)
{
	XmlNode commandsNode = xmlNode.findChild("nmt_messages");
	vector<XmlNode> commandNodes = commandsNode.getChildren();

	map<string, CanVariable> sortedData;

	for (int n = 0; n < commandNodes.size(); n++)
	{
		map<string, string> attributes = commandNodes[n].getAttributes();

		if (stoi(attributes["id"]) == commandId)
		{
			XmlNode varablesNode = commandNodes[n].findChild("variables");

			vector<XmlNode> variableNodes = varablesNode.getChildren();

			for (int c = 0; c < variableNodes.size(); c++)
			{
				map<string, string> attributes = variableNodes[c].getAttributes();

				if (data.find(attributes["name"]) != data.end())
				{
					int bitStart = stoi(attributes["start_bit"]);///FIXME: This is not a good way
					int bitLength = stoi(attributes["bit_length"]);

					sortedData[attributes["name"]].setType(attributes["type"]);
					sortedData[attributes["name"]].setUnit(attributes["unit"]);
					sortedData[attributes["name"]].setBitLength(bitLength);
					sortedData[attributes["name"]].setBitLength(bitLength);
					sortedData[attributes["name"]].setStartBit(bitStart);
					sortedData[attributes["name"]].setName(attributes["name"]);
					sortedData[attributes["name"]].setValue(data[attributes["name"]].getValue());
				}
			}
		}
	}

	string bin = "0000000000000000000000000000000000000000000000000000000000000000";
	int heighestBit = 0;

	map<string, CanVariable>::iterator iter;

	for (iter = sortedData.begin(); iter != sortedData.end(); iter++)///FIXME: We should check order and length
	{
		if (iter->second.getType() == "uint")
		{
			if (iter->second.getStartBit() + iter->second.getBitLength() > heighestBit)
				heighestBit = iter->second.getStartBit() + iter->second.getBitLength();

			//cout << iter->first << ":" <<  stoi(iter->second.getValue()) << endl;
			unsigned int a = stoi(iter->second.getValue());

			bin.replace(iter->second.getStartBit(), iter->second.getBitLength(), uint2bin(a, iter->second.getBitLength()));
		}
		else if (iter->second.getType() == "float")
		{
			if (iter->second.getStartBit() + iter->second.getBitLength() > heighestBit)
				heighestBit = iter->second.getStartBit() + iter->second.getBitLength();

			bin.replace(iter->second.getStartBit(), iter->second.getBitLength(), float2bin(stof(iter->second.getValue()), iter->second.getBitLength()));
		}
		else if (iter->second.getType() == "ascii")
		{
			//cout << iter->second.getValue() << endl;
			for (int n = 0; n < iter->second.getValue().length(); n++)
			{
				if (iter->second.getStartBit()+n*8 + 8 > heighestBit)
					heighestBit = iter->second.getStartBit()+n*8 + 8;

				//cout << iter->second.getValue()[n] << ":" << (unsigned int)iter->second.getValue()[n] << endl;
				bin.replace(iter->second.getStartBit()+n*8, 8, uint2bin((unsigned int)iter->second.getValue()[n], 8));
			}
		}
		else if (iter->second.getType() == "hexstring")
		{
			for (int n = 0; n < iter->second.getValue().length(); n++)
			{
				if (iter->second.getStartBit()+n*4 + 4 > heighestBit)
					heighestBit = iter->second.getStartBit()+n*4 + 4;

				string character;
				character += iter->second.getValue()[n];

				bin.replace(iter->second.getStartBit()+n*4, 4, hex2bin(character));
			}
		}
	}

	bin = bin.substr(0, heighestBit);

	//cout << bin2hex(bin) << endl;

	return bin2hex(bin);
}

