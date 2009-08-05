/*
 * Integer.h
 *
 *  Created on: Aug 05, 2009
 *      Author: Mattias Runge
 */

#ifndef INTEGER_H_
#define INTEGER_H_

#include "message/types/BasicType.hpp"

namespace atom {
namespace message {
namespace types {

class Integer : public BasicType<long>
{
public:
	Integer(xml::Node xmlNode) : BasicType<long>(xmlNode) { };


	string toString()
	{
		return convert::int2string(this->myValue);
	}
};

}
}
}

#endif // INTEGER_H_
