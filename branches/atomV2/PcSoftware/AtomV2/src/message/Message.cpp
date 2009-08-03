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

	xml::Node moduleNode = db::Database::getInstance()->getRootNode().selectFirst("modules").selectFirst("module", xml::Node::attributePair("name", moduletype));
	xml::Node messageNode = db::Database::getInstance()->getRootNode().selectFirst("messagetypes").selectFirst("messagetype", xml::Node::attributePair("name", type));

	xml::Node::nodeList variableNodes = messageNode.select("variable");

	for (unsigned int n = 0; n < variableNodes.size(); n++)
	{
		Variable variable(variableNodes[n]["name"], variableNodes[n]["datatype"], stob(variableNodes[n]["required"]), variableNodes[n]["unit"]);
		this->myVariables[variableNodes[n]["name"]] = variable;
	}

	if (moduleNode.selectFirst("in").select("message", xml::Node::attributePair("name", type)).size() > 0)
	{
		this->myFromModule = false;
	}
	else if (moduleNode.selectFirst("out").select("message", xml::Node::attributePair("name", type)).size() > 0)
	{
		this->myFromModule = true;
	}
	else
	{
		LOG.error("This message is not supported by this module type, something is wrong; protocol.xml is old, module code is old or a Atom is trying to send and illegal message.");
		// TODO throw exception
	}

	xml::Node::nodeList responseNodes = messageNode.select("response");

	for (unsigned int n = 0; n < responseNodes.size(); n++)
	{
		this->myResponses.push_back(responseNodes[n]["code"]); // TODO Validate the code
	}

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
