/*
 * Datatype.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#include "Datatype.h"

#include "message/types/UnsignedInteger.hpp"
#include "message/types/Integer.hpp"
#include "message/types/Boolean.hpp"
#include "message/types/Decimal.hpp"
#include "message/types/Enum.hpp"

namespace atom {
namespace message {

Datatype::Datatype(xml::Node xmlNode)
{
	LOG.setName("Datatype");

	this->myType = xmlNode["type"];
	this->myName = xmlNode["name"];
	this->myLength = convert::string2uint(xmlNode["length"]); // Not all types have length specified, like enum where it is calculated... it is then 0 here and set in the subclass
}

Datatype::~Datatype()
{
}

void Datatype::readBits(BitBuffer & buffer)
{
	LOG.warn("Datatype::readBits should be overridden by subclass!");
}

void Datatype::writeBits(BitBuffer & buffer)
{
	LOG.warn("Datatype::writeBits should be overridden by subclass!");
}

Datatype::pointer Datatype::create(string name)
{
	xml::Node xmlNode = Protocol::getInstance()->getRootNode().selectFirst("datatypes").selectFirst("datatype", xml::Node::attributePair("name", name));

	string type = xmlNode["type"];

	if (type == "unsigned integer")
	{
		return boost::make_shared<types::UnsignedInteger>(xmlNode);
	}
	else if (type == "integer")
	{
		return boost::make_shared<types::Integer>(xmlNode);
	}
	else if (type == "boolean")
	{
		return boost::make_shared<types::Boolean>(xmlNode);
	}
	else if (type == "decimal")
	{
		return boost::make_shared<types::Decimal>(xmlNode);
	}
	else if (type == "enum")
	{
		return boost::make_shared<types::Enum>(xmlNode);
	}
	/*else if (type == "string")
	{
		return boost::make_shared<Datatype>(new types::UnsignedInteger(xmlNode));
	}
	else if (type == "response")
	{
		return boost::make_shared<Datatype>(new types::UnsignedInteger(xmlNode));
	}*/

	// TODO throw exception here!

	return boost::make_shared<Datatype>(xmlNode);
}

string Datatype::toString()
{
	return "<empty>";
}

}
}
