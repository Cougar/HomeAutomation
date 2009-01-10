/***************************************************************************
 *   Copyright (C) November 30, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   xmlnode.h                                            *
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

#ifndef _XMLNODE_H
#define	_XMLNODE_H

using namespace std;

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "xmlexception.h"
#include "../Tools/tools.h"

#include <iostream>

class XmlNode
{
public:
	XmlNode() {};
	XmlNode(string filename) { load(filename); };

	string getTagName() { return myTagName; }
	map<string, string> getAttributes() { return myAttributes; };
	vector<XmlNode> getChildren() { return myChildren; };

	void load(string filename);
	void parse(string xmlData);
	XmlNode findChild(string tagName);
	string getAttributeValue(string attributeName);

protected:
	int parseTag(string xmlData, int position);

private:
	string myTagName;
	map<string, string> myAttributes;
	vector<XmlNode> myChildren;
};

#endif	/* _XMLNODE_H */

