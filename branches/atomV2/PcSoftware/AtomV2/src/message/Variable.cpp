/*
 * Variable.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#include "Variable.h"

namespace atom {
namespace message {

Variable::Variable(xml::Node xmlNode)
{
	this->myName = xmlNode["name"];
	this->myRequired = convert::string2bool(xmlNode["required"]);
	this->myUnit = xmlNode["unit"];

	this->myDatatype = Datatype::create(xmlNode["datatype"]);
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

string Variable::toString()
{
	string buffer = this->myName + "=" + this->myDatatype->toString();

	if (this->myUnit != "")
	{
		buffer += " " + this->myUnit;
	}

	return buffer;
}

}
}
