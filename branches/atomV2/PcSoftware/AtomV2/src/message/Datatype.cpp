/*
 * Datatype.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#include "Datatype.h"

namespace atom {
namespace message {

Datatype::Datatype(datatypes type)
{
	LOG.setName("Datatype");
	this->myType = type;
}

Datatype::~Datatype()
{
}

boost::any Datatype::getValue()
{
	return this->myValue;
}

void Datatype::setValue(boost::any value)
{
	this->myValue = value;
}

void Datatype::readBits(BitBuffer & buffer, unsigned int length)
{


}

datatypes Datatype::getType()
{
	return this->myType;
}

datatypes Datatype::getTypeFromString(string name)
{
	if (name == "unsigned integer")
	{
		return DATATYPE_UNSIGNED_INTEGER;
	}
	else if (name == "integer")
	{
		return DATATYPE_INTEGER;
	}
	else if (name == "decimal")
	{
		return DATATYPE_DECIMAL;
	}
	else if (name == "boolean")
	{
		return DATATYPE_BOOLEAN;
	}
	else if (name == "string")
	{
		return DATATYPE_STRING;
	}
	else if (name == "enum")
	{
		return DATATYPE_ENUM;
	}
	else if (name == "response")
	{
		return DATATYPE_RESPONSE;
	}

	return DATATYPE_UNKNOWN;
}

}
}
