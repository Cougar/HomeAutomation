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

}
}
