/*
 * Message.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#include "Message.h"

namespace atom {
namespace message {

Message::Message(string moduletype, unsigned int moduleId, string type)
{
	LOG.setName("Message");

	this->myModuletype = moduletype;
	this->myModuleId = moduleId;
	this->myType = type;

	// TODO Check if this message is supported for the module

	// TODO Find if this is in our out on module, if out then set myFromModule = true

	// TODO Convert type into variables
	// TODO Convert type into responses
}

Message::Message()
{
}

Message::~Message()
{
}

void Message::setOrigin(const void *origin)
{
	//LOG.debug("setOrigin(" + itos((unsigned long) origin) + ")");
	this->myOrigin = origin;
}

bool Message::isOrigin(const void *origin)
{
	//LOG.debug("isOrigin(" + itos((unsigned long)origin) + "==" + itos((unsigned long) this->myOrigin) + ")");
	return this->myOrigin == origin;
}


bool Message::isFromModule()
{
	return this->myFromModule;
}

string Message::getModuletype()
{
	return this->myModuletype;
}

unsigned int Message::getModuleId()
{
	return this->myModuleId;
}

string Message::getType()
{
	return this->myType;
}

Variable & Message::getVariable(string name)
{
	return this->myVariables[name];

	// TODO Check if it does not exist, throw exception?
}

void Message::readBits(BitBuffer & buffer)
{
	for (variableMap::iterator iter = this->myVariables.begin(); iter != this->myVariables.end(); iter++)
	{
		iter->second.readBits(buffer);
	}
}


Message::responseList Message::getResponses()
{
	return this->myResponses;
}

}
}
