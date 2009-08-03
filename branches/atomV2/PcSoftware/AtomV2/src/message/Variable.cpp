/*
 * Variable.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#include "Variable.h"

namespace atom {
namespace message {

Variable::Variable(string datatype, bool required, string unit)
{
	this->myRequired = required;
	this->myUnit = unit;

	// TODO convert datatype-string into datatype and length
	// this->myLength;
	// this->myDatatype = Datatype_create(type);
}

Variable::Variable()
{
}

Variable::~Variable()
{
	// TODO Auto-generated destructor stub
}

void Variable::readBits(BitBuffer & buffer)
{
	(*this->myDatatype).readBits(buffer, this->myLength);
}

}
}
