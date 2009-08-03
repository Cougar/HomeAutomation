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
	// TODO Load variables from headerfile
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
