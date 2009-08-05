/*
 * Header.cpp
 *
 *  Created on: Jul 23, 2009
 *      Author: Mattias Runge
 */

#include "Header.h"

#include "message/types/UnsignedInteger.hpp"

namespace atom {
namespace message {

Header::Header()
{
}

Header::Header(BitBuffer & buffer)
{
	this->readBits(buffer);
}

Header::Header(unsigned long moduleId, string moduleType, string messageType)
{
	if (types::UnsignedInteger *moduleIdDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["moduleId"].getDatatype().get()))
	{
		moduleIdDatatype->setValue(moduleId);
	}
	else
	{
		// TODO throw exception
	}

	if (types::UnsignedInteger *moduleTypeDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["moduleType"].getDatatype().get()))
	{
		unsigned long moduleTypeId;

		// TODO convert moduleType to moduleTypeId

		moduleTypeDatatype->setValue(moduleTypeId);
	}
	else
	{
		// TODO throw exception
	}

	if (types::UnsignedInteger *messageTypeDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["messageType"].getDatatype().get()))
	{
		unsigned long messageTypeId;

		// TODO convert messageType to messageTypeId

		messageTypeDatatype->setValue(messageTypeId);
	}
	else
	{
		// TODO throw exception
	}
}

Header::~Header()
{
}

void Header::loadVariables()
{
	xml::Node::nodeList nodes = Protocol::getInstance()->getRootNode().selectFirst("header").select("variable");

	for (unsigned int n = 0; n < nodes.size(); n++)
	{
		Variable variable(nodes[n]);
		this->myVariables[nodes[n]["name"]] = variable;
	}

	// TODO check that moduleType, moduleId and messageId are present
}

unsigned long Header::getModuleId()
{
	if (types::UnsignedInteger *moduleIdDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["moduleId"].getDatatype().get()))
	{
		return moduleIdDatatype->getValue();
	}
	else
	{
		// TODO throw exception
	}
}

string Header::getModuleType()
{
	if (types::UnsignedInteger *moduleTypeDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["moduleType"].getDatatype().get()))
	{
		unsigned long moduleTypeId = moduleTypeDatatype->getValue();

		string moduleType;

		// TODO convert moduleTypeId to moduleType

		return moduleType;
	}
	else
	{
		// TODO throw exception
	}
}

string Header::getMessageType()
{
	if (types::UnsignedInteger *messageTypeDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["messageType"].getDatatype().get()))
	{
		unsigned long messageTypeId = messageTypeDatatype->getValue();

		string messageType;

		// TODO convert messageTypeId to messageType

		return messageType;
	}
	else
	{
		// TODO throw exception
	}
}

void Header::readBits(BitBuffer & buffer)
{
	// Header is only 29 bits but our buffer is whole bytes, padded in front
	buffer.incrementPointer(3); // Increment 3 bits

	for (variableMap::iterator iter = this->myVariables.begin(); iter != this->myVariables.end(); iter++)
	{
		iter->second.readBits(buffer);
	}
}

void Header::writeBits(BitBuffer & buffer)
{
	// Header is only 29 bits but our buffer is whole bytes, padded in front
	buffer.writeBasicType(3, (long) 0); // Write 3 bits

	for (variableMap::iterator iter = this->myVariables.begin(); iter != this->myVariables.end(); iter++)
	{
		iter->second.writeBits(buffer);
	}
}

}
}
