/*
 * Message.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#include "Message.h"

namespace atom {
namespace message {

Message::Message(Header header)
{
	LOG.setName("Message");

	this->myHeader = header;

	xml::Node messageNode = Protocol::getInstance()->getRootNode().selectFirst("messagetypes").selectFirst("messagetype", xml::Node::attributePair("name", this->myHeader.getMessageType()));

	// Load variables
	xml::Node::nodeList variableNodes = messageNode.select("variable");

	for (unsigned int n = 0; n < variableNodes.size(); n++)
	{
		Variable variable(variableNodes[n]);
		this->myVariables[variableNodes[n]["name"]] = variable;
	}

	// Load responses
	xml::Node::nodeList responseNodes = messageNode.select("response");

	for (unsigned int n = 0; n < responseNodes.size(); n++)
	{
		this->myResponses.push_back(responseNodes[n]["code"]); // TODO Validate the code
	}

	// Check if valid for this module
	xml::Node moduleNode = Protocol::getInstance()->getRootNode().selectFirst("modules").selectFirst("module", xml::Node::attributePair("name", this->myHeader.getModuleType()));

	if (moduleNode.selectFirst("in").select("message", xml::Node::attributePair("name", this->myHeader.getModuleType())).size() == 0 &&
		moduleNode.selectFirst("out").select("message", xml::Node::attributePair("name", this->myHeader.getModuleType())).size() == 0)
	{
		LOG.error("This message is not supported by this module type, something is wrong; protocol.xml is old, module code is old or a Atom is trying to send and illegal message.");
		// TODO throw exception
	}
}

Message::Message()
{
	LOG.setName("Message");
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


Header & Message::getHeader()
{
	return this->myHeader;
}

Variable & Message::getVariable(string name)
{
	return this->myVariables[name];

	// TODO Check if it does not exist, throw exception?
}

Message::responseList Message::getResponses()
{
	return this->myResponses;
}


void Message::readBits(BitBuffer & buffer)
{
	for (variableMap::iterator iter = this->myVariables.begin(); iter != this->myVariables.end(); iter++)
	{
		iter->second.readBits(buffer);
	}
}

void Message::writeBits(BitBuffer & buffer)
{
	for (variableMap::iterator iter = this->myVariables.begin(); iter != this->myVariables.end(); iter++)
	{
		iter->second.writeBits(buffer);
	}
}


}
}
