/*
 * Boolean.h
 *
 *  Created on: Aug 05, 2009
 *      Author: Mattias Runge
 */

#ifndef BOOLEAN_H_
#define BOOLEAN_H_

#include "message/types/BasicType.hpp"

namespace atom {
namespace message {
namespace types {

class Boolean : public BasicType<bool>
{
public:
	Boolean(xml::Node xmlNode) : BasicType<bool>(xmlNode)
	{
		this->myLength = 1;
	};

	string toString()
	{
		return convert::bool2string(this->myValue);
	}
};

}
}
}

#endif // BOOLEAN_H_
