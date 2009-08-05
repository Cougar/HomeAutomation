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
	LOG.setName("Header");
	this->loadVariables();
}

Header::Header(BitBuffer & buffer)
{
	LOG.setName("Header");
	this->loadVariables();
	this->readBits(buffer);
}

Header::Header(unsigned long moduleId, string moduleType, string messageType)
{
	LOG.setName("Header");

	LOG.debug("Start of constructor...");

	this->loadVariables();

	LOG.debug("Done loading variables...");

	if (types::UnsignedInteger *moduleIdDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["moduleId"].getDatatype().get()))
	{
		moduleIdDatatype->setValue(moduleId);
	}
	else
	{
		throw new Exception("moduleId is not of the type \"unsigned integer\", this is a fatal error, correct the protocol specification");
	}

	LOG.debug("checkpoint 1...");

	if (types::UnsignedInteger *moduleTypeDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["moduleType"].getDatatype().get()))
	{
		unsigned long moduleTypeId = convert::string2uint(Protocol::getInstance()->getRootCacheNode().selectFirst("module", xml::Node::attributePair("name", moduleType))["id"]);

		moduleTypeDatatype->setValue(moduleTypeId);
	}
	else
	{
		throw new Exception("moduletype is not of the type \"unsigned integer\", this is a fatal error, correct the protocol specification");
	}

	LOG.debug("checkpoint 2...");

	if (types::UnsignedInteger *messageTypeDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["messageType"].getDatatype().get()))
	{
		unsigned long messageTypeId = convert::string2uint(Protocol::getInstance()->getRootCacheNode().selectFirst("messagetype", xml::Node::attributePair("name", messageType))["id"]);

		messageTypeDatatype->setValue(messageTypeId);
	}
	else
	{
		throw new Exception("messageType is not of the type \"unsigned integer\", this is a fatal error, correct the protocol specification");
	}

	LOG.debug("End of contructor...");
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

	throw new Exception("moduleIdDatatype is not of the type \"unsigned integer\", this is a fatal error, correct the protocol specification");
}

string Header::getModuleType()
{
	if (types::UnsignedInteger *moduleTypeDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["moduleType"].getDatatype().get()))
	{
		unsigned long moduleTypeId = moduleTypeDatatype->getValue();

		string moduleType = Protocol::getInstance()->getRootCacheNode().selectFirst("module", xml::Node::attributePair("id", convert::uint2string(moduleTypeId)))["name"];

		return moduleType;
	}

	throw new Exception("moduleType is not of the type \"unsigned integer\", this is a fatal error, correct the protocol specification");
}

string Header::getMessageType()
{
	if (types::UnsignedInteger *messageTypeDatatype = dynamic_cast<types::UnsignedInteger *>(this->myVariables["messageType"].getDatatype().get()))
	{
		unsigned long messageTypeId = messageTypeDatatype->getValue();

		string messageType = Protocol::getInstance()->getRootCacheNode().selectFirst("messagetype", xml::Node::attributePair("id", convert::uint2string(messageTypeId)))["name"];

		return messageType;
	}

	throw new Exception("messageType is not of the type \"unsigned integer\", this is a fatal error, correct the protocol specification");
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
	buffer.write(3, (long) 0); // Write 3 bits

	for (variableMap::iterator iter = this->myVariables.begin(); iter != this->myVariables.end(); iter++)
	{
		iter->second.writeBits(buffer);
	}
}

string Header::toString()
{
	string buffer = "Header(";

	for (variableMap::iterator iter = this->myVariables.begin(); iter != this->myVariables.end(); iter++)
	{
		buffer += iter->second.toString() + ",";
	}

	trim_if(buffer, is_any_of(","));

	buffer += ")";

	return buffer;
}

}
}
