/*
 * UnsignedInteger.h
 *
 *  Created on: Aug 05, 2009
 *      Author: Mattias Runge
 */

#ifndef UNSIGNEDINTEGER_H_
#define UNSIGNEDINTEGER_H_

#include "message/types/BasicType.hpp"

namespace atom {
namespace message {
namespace types {

class UnsignedInteger : public BasicType<unsigned long>
{
public:
	UnsignedInteger(xml::Node xmlNode) : BasicType<unsigned long>(xmlNode) { };


	string toString()
	{
		return convert::uint2string(this->myValue);
	}
};

}
}
}

#endif // UNSIGNEDINTEGER_H_
