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

	xml::Node datatypeNode = Protocol::getInstance()->getRootNode().selectFirst("datatypes").selectFirst("datatype", xml::Node::attributePair("name", datatype));

	this->myLength = convert::string2uint(datatypeNode["length"]);
	this->myDatatype = boost::make_shared<Datatype>(Datatype::getTypeFromString(datatypeNode["type"]));
}

Variable::Variable()
{
}

Variable::~Variable()
{
}

boost::any Variable::getValue()
{
	return this->myDatatype->getValue();
}

void Variable::setValue(boost::any value)
{
	this->myDatatype->setValue(value);
}

void Variable::readBits(BitBuffer & buffer)
{
	this->myDatatype->readBits(buffer, this->myLength);
}

}
}
