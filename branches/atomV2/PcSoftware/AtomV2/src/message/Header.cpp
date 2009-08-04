/*
 * Header.cpp
 *
 *  Created on: Jul 23, 2009
 *      Author: Mattias Runge
 */

#include "Header.h"

namespace atom {
namespace message {

Header::Header()
{
	xml::Node::nodeList nodes = Protocol::getInstance()->getRootNode().selectFirst("header").select("variable");

	for (unsigned int n = 0; n < nodes.size(); n++)
	{
		Variable variable(nodes[n]["name"], nodes[n]["datatype"], stob(nodes[n]["required"]), nodes[n]["unit"]);
		this->myVariables[nodes[n]["name"]] = variable;
	}
}

Header::~Header()
{
}

Variable & Header::getVariable(string name)
{
	return this->myVariables[name];

	// TODO Check if it does not exist, throw exception?
}

void Header::readBits(BitBuffer & buffer)
{
	for (variableMap::iterator iter = this->myVariables.begin(); iter != this->myVariables.end(); iter++)
	{
		iter->second.readBits(buffer);
	}
}

}
}
