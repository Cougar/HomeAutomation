/*
 * Node.cpp
 *
 *  Created on: Apr 26, 2009
 *      Author: Mattias Runge
 */

#include "Node.h"

namespace atom {
namespace xml {

Node::Node()
{
	this->myIsEmpty = true;
	LOG.setName("XML::Node");
}

Node::Node(string filename)
{
	this->myIsEmpty = true;
	LOG.setName("XML::Node");
	this->load(filename);
}

Node::Node(string filename, string xmlData)
{
	this->myIsEmpty = true;
	LOG.setName("XML::Node");
	this->parse(filename, xmlData);
}

void Node::load(string filename)
{
	this->parse(filename, file::get_contents(filename));
}

void Node::parse(string filename, string xmlData)
{
	char c;
	unsigned int position = 0;

	while (position < xmlData.length())
	{
		c = xmlData.at(position);

		switch (c)
		{
		case '<':
			position = this->parseTag(filename, xmlData, position);
			break;
		}

		position++;
	}
}

string Node::tagName()
{
	return this->myTagName;
}

Node::attributeList & Node::attributes()
{
	return this->myAttributes;
}

string Node::operator [](string attributeName)
{
	attributeList::iterator iter = this->myAttributes.find(attributeName);

	if (iter == this->myAttributes.end())
	{
		return "";
	}

	return iter->second;
}

Node Node::selectFirst()
{
	// TODO Throw exception if returned list is empty
	return this->select()[0];
}

Node Node::selectFirst(string tagName)
{
	// TODO Throw exception if returned list is empty
	return this->select(tagName)[0];
}

Node Node::selectFirst(string tagName, attributeList attributes)
{
	// TODO Throw exception if returned list is empty
	return this->select(tagName, attributes)[0];
}

Node Node::selectFirst(string tagName, attributePair pair1)
{
	attributeList attributes;
	attributes.insert(pair1);
	return this->selectFirst(tagName, attributes);
}

Node Node::selectFirst(string tagName, attributePair pair1, attributePair pair2)
{
	attributeList attributes;
	attributes.insert(pair1);
	attributes.insert(pair2);
	return this->selectFirst(tagName, attributes);
}

Node Node::selectFirst(string tagName, attributePair pair1, attributePair pair2, attributePair pair3)
{
	attributeList attributes;
	attributes.insert(pair1);
	attributes.insert(pair2);
	attributes.insert(pair3);
	return this->selectFirst(tagName, attributes);
}

Node Node::selectFirst(string tagName, attributePair pair1, attributePair pair2, attributePair pair3, attributePair pair4)
{
	attributeList attributes;
	attributes.insert(pair1);
	attributes.insert(pair2);
	attributes.insert(pair3);
	attributes.insert(pair4);
	return this->selectFirst(tagName, attributes);
}

Node Node::selectFirst(string tagName, attributePair pair1, attributePair pair2, attributePair pair3, attributePair pair4, attributePair pair5)
{
	attributeList attributes;
	attributes.insert(pair1);
	attributes.insert(pair2);
	attributes.insert(pair3);
	attributes.insert(pair4);
	attributes.insert(pair5);
	return this->selectFirst(tagName, attributes);
}

Node::nodeList Node::select()
{
	return this->myChildren;
}

Node::nodeList Node::select(string tagName)
{
	attributeList attributes;
	return this->select(tagName, attributes);
}

Node::nodeList Node::select(string tagName, attributeList attributes)
{
	nodeList list;
	bool attributesMatch = true;

	for (unsigned int n = 0; n < this->myChildren.size(); n++)
	{
		if (this->myChildren[n].tagName() == tagName)
		{
			attributesMatch = true;

			for (attributeList::iterator iter = attributes.begin(); iter != attributes.end(); iter++)
			{
				//LOG.debug("#attributes = " + convert::int2string(this->myChildren[n].attributes().size()));
				//LOG.debug("iter->first = " + iter->first);
				//LOG.debug("iter->second = " + iter->second);
				//LOG.debug("this->myChildren[n][iter->first] = " + this->myChildren[n][iter->first]);
				if (this->myChildren[n][iter->first] != iter->second)
				{
					attributesMatch = false;
					break;
				}
			}

			if (attributesMatch)
			{
				list.push_back(myChildren[n]);
			}
		}
	}

	return list;
}


Node::nodeList Node::select(string tagName, attributePair pair1)
{
	attributeList attributes;
	attributes.insert(pair1);
	return this->select(tagName, attributes);
}

Node::nodeList Node::select(string tagName, attributePair pair1, attributePair pair2)
{
	attributeList attributes;
	attributes.insert(pair1);
	attributes.insert(pair2);
	return this->select(tagName, attributes);
}

Node::nodeList Node::select(string tagName, attributePair pair1, attributePair pair2, attributePair pair3)
{
	attributeList attributes;
	attributes.insert(pair1);
	attributes.insert(pair2);
	attributes.insert(pair3);
	return this->select(tagName, attributes);
}

Node::nodeList Node::select(string tagName, attributePair pair1, attributePair pair2, attributePair pair3, attributePair pair4)
{
	attributeList attributes;
	attributes.insert(pair1);
	attributes.insert(pair2);
	attributes.insert(pair3);
	attributes.insert(pair4);
	return this->select(tagName, attributes);
}

Node::nodeList Node::select(string tagName, attributePair pair1, attributePair pair2, attributePair pair3, attributePair pair4, attributePair pair5)
{
	attributeList attributes;
	attributes.insert(pair1);
	attributes.insert(pair2);
	attributes.insert(pair3);
	attributes.insert(pair4);
	attributes.insert(pair5);
	return this->select(tagName, attributes);
}

int Node::parseTag(string filename, string xmlData, unsigned int position)
{
	if (xmlData.at(position) != '<')
	{
		throw new Exception("Did not find < as expected. Character " + convert::int2string(position));
	}

	position++;

	int endPosition = xmlData.find('>', position);

	if (xmlData.at(position) == '?' || xmlData.at(position) == '!' || xmlData.at(position) == '/')
	{
		return endPosition;
	}

	this->myIsEmpty = false;

	string tagString;

	try
	{
		tagString = xmlData.substr(position, endPosition - position);
	}
	catch (std::out_of_range & e)
	{
		LOG.debug("parseTag exception: " + string(e.what()));
		LOG.debug("A xmlData=" + xmlData + " :: position=" + convert::int2string(position) + " :: endPosition=" + convert::int2string(endPosition));
		throw e;
	}

	trim(tagString);
	trim_if(tagString, is_any_of("\n"));
	trim(tagString);
	tagString += " ";

	bool foundEnd = false;
	bool foundTag = false;
	string buffer;
	unsigned int pos = 0;
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
				this->myTagName = buffer;
				buffer = "";
				//cout << "Found tag: " << myTagName << endl;
				foundTag = true;
			}
			break;

		case '/':
			foundEnd = true;
			break;

		case '=':
			s = tagString.find('"', pos) + 1;
			e = tagString.find('"', s);

			try
			{
				this->myAttributes[buffer] = tagString.substr(s, e - s);
				//LOG.debug("Added attribute \"" + buffer + "\" = \"" + this->myAttributes[buffer] + "\"");
			}
			catch (std::out_of_range & ex)
			{
				LOG.debug("parseTag exception: " + string(ex.what()));
				LOG.debug("A xmlData=" + xmlData + " :: position=" + convert::int2string(position) + " :: endPosition=" + convert::int2string(endPosition));
				throw ex;
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
				if (xmlData.at(position + 1) == '/')
				{
					position += 2;
					endPosition = xmlData.find('>', position);

					try
					{
						if (myTagName.compare(xmlData.substr(position, endPosition - position)) == 0)
						{
							//cout << "Found end tag: " << myTagName << endl;
							return endPosition;
						}
						else
						{
							throw new Exception("Invalid end tag found. " + xmlData.substr(position, endPosition - position) + "::" + myTagName);
						}
					}
					catch (std::out_of_range & ex)
					{
						LOG.debug("parseTag exception: " + string(ex.what()));
						LOG.debug("A xmlData=" + xmlData + " :: position=" + convert::int2string(position) + " :: endPosition=" + convert::int2string(endPosition));
						throw ex;
					}

				}

				Node subNode;
				position = subNode.parseTag(filename, xmlData, position);

				if (!subNode.myIsEmpty)
				{
					this->myChildren.push_back(subNode);
				}

				break;
			}

			position++;
		}
	}

	return position;
}

string Node::toString()
{
	string buffer = "<" + this->myTagName;

	for (attributeList::iterator iter = this->myAttributes.begin(); iter != this->myAttributes.end(); iter++)
	{
		buffer += " " + iter->first + "=\"" + iter->second + "\"";
	}

	if (this->myChildren.size() > 0)
	{
		buffer += ">\n";

		for (unsigned int n = 0; n < this->myChildren.size(); n++)
		{
			buffer += this->myChildren[n].toString();
		}

		buffer += "</" + this->myTagName + ">\n";
	}
	else
	{
		buffer += "/>\n";
	}

	return buffer;
}

}
}
