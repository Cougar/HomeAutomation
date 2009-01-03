/***************************************************************************
 *   Copyright (C) November 28, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   canidtranslator.h                                            *
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

#ifndef _CANIDTRANSLATOR_H
#define	_CANIDTRANSLATOR_H

using namespace std;

#include <string>
#include <map>

#include "canvariable.h"
#include "cantranslatorexception.h"
#include "../Settings/settings.h"
#include "../Xml/xmlnode.h"

class CanIdTranslator
{
public:
	static CanIdTranslator& getInstance();
	static void deleteInstance();

	string lookupClassName(int classId);
	int resolveClassId(string className);

	string lookupDirectionFlag(int directionFlag);
	int resolveDirectionFlag(string directionName);

	string lookupModuleName(int moduleId);
	int resolveModuleId(string moduleName);

	string lookupCommandName(int commandId, string moduleName);
	int resolveCommandId(string commandName, string moduleName);

	string getDefineId(string name, string group);

	map<string, CanVariable> translateData(int commandId, string moduleName, string dataHex);
	string translateDataToHex(int commandId, string moduleName, map<string, CanVariable> &data);
	void makeDataValid(int commandId, string moduleName, map<string, CanVariable> &data);

	string lookupNMTCommandName(int commandId);
	int resolveNMTCommandId(string commandName);

	map<string, CanVariable> translateNMTData(int commandId, string rawHexData);
	string translateNMTDataToHex(int commandId, map<string, CanVariable> &data);
	void makeNMTDataValid(int commandId, map<string, CanVariable> &data);

protected:
	CanIdTranslator();

private:
	static CanIdTranslator* myInstance;
	XmlNode xmlNode;

	void makeDataValid(vector<XmlNode> variableNodes, map<string, CanVariable> &data);
	string translateValidDataToHex(map<string, CanVariable> &data);
	map<string, CanVariable> translateData(vector<XmlNode> variableNodes, string dataHex);
};

#endif	/* _CANIDTRANSLATOR_H */

