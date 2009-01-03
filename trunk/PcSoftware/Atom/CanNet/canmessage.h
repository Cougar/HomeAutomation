/***************************************************************************
 *   Copyright (C) November 28, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   canmessage.h                                            *
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

#ifndef _CANMESSAGE_H
#define	_CANMESSAGE_H

using namespace std;

#include <string>
#include <map>

#include "canmessageexception.h"
#include "canidtranslator.h"
#include "canvariable.h"
#include "../Tools/tools.h"

class CanMessage
{
public:
	CanMessage() { myIsUnknown = false; myModuleId = 0; };
	CanMessage(string raw) { setRaw(raw); };

	void setRaw(string rawHex);
	string getRaw();

	bool isUnknown() { return myIsUnknown; };
	
	string getClassName() { return myClassName; };
	void setClassName(string className) { myClassName = className; }; ///FIXME: Check if data is valid

	string getDirectionFlag() { return myDirectionFlag; };
	void setDirectionFlag(string directionFlag) { myDirectionFlag = directionFlag; }; ///FIXME: Check if data is valid

	string getModuleName() { return myModuleName; };
	void setModuleName(string moduleName) { myModuleName = moduleName; }; ///FIXME: Check if data is valid

	unsigned int getModuleId() { return myModuleId; };
	void setModuleId(unsigned int moduleId) { myModuleId = moduleId; };

	string getCommandName() { return myCommandName; };
	void setCommandName(string commandName) { myCommandName = commandName; }; ///FIXME: Check if data is valid

	map<string, CanVariable>& getData() { return myData; };
	string getJSONData();
	void setData(map<string, CanVariable> data);
	
private:
	bool myIsUnknown;
	string myClassName;
	string myDirectionFlag;
	string myModuleName;
	unsigned int myModuleId;
	string myCommandName;
	map<string, CanVariable> myData;
};

#endif	/* _CANMESSAGE_H */

