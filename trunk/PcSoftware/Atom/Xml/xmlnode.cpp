/***************************************************************************
 *   Copyright (C) November 30, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   xmlnode.cpp                                            *
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

#include "xmlnode.h"

void XmlNode::load(string filename)
{
	parse(file_get_contents(filename));
}

void XmlNode::parse(string xmlData)
{
	char c;
	int position = 0;
	while (position < xmlData.length())
	{
		c = xmlData.at(position);

		switch (c)
		{
			case '<':
				position = parseTag(xmlData, position);
				break;
		}
		position++;
	}
}

int XmlNode::parseTag(string xmlData, int position)
{
	if (xmlData.at(position) != '<')
		throw new XmlException("Did not find < as expected. Character " + itos(position));

	position++;

	int endPosition = xmlData.find('>', position);

	if (xmlData.at(position) == '?' || xmlData.at(position) == '!' || xmlData.at(position) == '/')
	{
		return endPosition;
	}

	string tagString;

	try
	{
		tagString = xmlData.substr(position, endPosition-position);
	}
	catch (std::out_of_range& e)
	{
		cout << "DEBUG: parseTag exception: " << e.what() << "\n";
		cout << "DEBUG: A xmlData=" << xmlData << " :: position=" << position << " :: endPosition=" << endPosition << endl;
	}

	tagString = trim(tagString);
	tagString = trim(tagString, '\n');
	tagString = trim(tagString);
	tagString += " ";

	bool foundEnd = false;
	bool foundTag = false;
	string buffer;
	int pos = 0;
	char c;
	int s, e;

	while (pos < tagString.length())
	{
		c = tagString.at(pos);

		switch (c)
		{
			case ' ':
				if (!foundTag)
				{
					myTagName = buffer;
					buffer = "";
					//cout << "Found tag: " << myTagName << endl;
					foundTag = true;
				}
				break;

			case '/':
				foundEnd = true;
				break;

			case '=':
				s = tagString.find('"', pos)+1;
				e = tagString.find('"', s);

				try
				{
					myAttributes[buffer] = tagString.substr(s, e-s);
				}
				catch (std::out_of_range& ex)
				{
					cout << "DEBUG: parseTag exception: " << ex.what() << "\n";
					cout << "DEBUG: B tagString=" << tagString << " :: s=" << s << " :: e=" << e << endl;
					continue;
				}


				//cout << "Found attribute: " << buffer << " = " << myAttributes[buffer] << endl;
				pos = e;
				buffer = "";
				break;

			default:
				buffer += c;
				break;
		}
		pos++;
	}
	
	position = endPosition;

	if (!foundEnd)
	{
		while (position < xmlData.length())
		{
			c = xmlData.at(position);

			switch (c)
			{
				case '<':
					if (xmlData.at(position+1) == '/')
					{
						position += 2;
						endPosition = xmlData.find('>', position);

						try
						{
							if (myTagName.compare(xmlData.substr(position, endPosition-position)) == 0)
							{
								//cout << "Found end tag: " << myTagName << endl;
								return endPosition;
							}
							else
							{
								throw new XmlException("Invalid end tag found. " + xmlData.substr(position, endPosition-position) + "::" + myTagName);
							}
						}
						catch (std::out_of_range& e)
						{
							cout << "DEBUG: parseTag exception: " << e.what() << "\n";
							cout << "DEBUG: C xmlData=" << xmlData << " :: position=" << position << " :: endPosition=" << endPosition << endl;
							continue;
						}

						
					}

					XmlNode subNode;
					position = subNode.parseTag(xmlData, position);
					myChildren.push_back(subNode);
					break;
			}

			position++;
		}
	}

	return position;
}

XmlNode XmlNode::findChild(string tagName)
{
	for (int n = 0; n < myChildren.size(); n++)
	{
		if (myChildren[n].getTagName().compare(tagName) == 0)
			return myChildren[n];
	}

	XmlNode defaultNode;

	return defaultNode;
}

string XmlNode::getAttributeValue(string attributeName)
{
	map<string, string>::const_iterator iterator = myAttributes.find(attributeName);

	if (iterator == myAttributes.end())
		return NULL;

	return iterator->second;
}
