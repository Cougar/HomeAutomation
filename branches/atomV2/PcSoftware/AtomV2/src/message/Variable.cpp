/*
 * Variable.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#include "Variable.h"

namespace atom {
namespace message {

Variable::Variable(string name, string datatype, bool required, string unit)
{
	this->myName = name;
	this->myRequired = required;
	this->myUnit = unit;

	this->myDatatype = Datatype::create(datatype);
}

Variable::Variable()
{
}

Variable::~Variable()
{
}

string Variable::getName()
{
	return this->myName;
}

Datatype::pointer Variable::getDatatype()
{
	return this->myDatatype;
}

string Variable::getUnit()
{
	return this->myUnit;
}

bool Variable::isRequired()
{
	return this->myRequired;
}

void Variable::readBits(BitBuffer & buffer)
{
	this->myDatatype->readBits(buffer);
}

void Variable::writeBits(BitBuffer & buffer)
{
	this->myDatatype->writeBits(buffer);
}

}
}
