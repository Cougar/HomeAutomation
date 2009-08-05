/*
 * BasicType.hpp
 *
 *  Created on: Aug 05, 2009
 *      Author: Mattias Runge
 */

#ifndef BASICTYPE_HPP_
#define BASICTYPE_HPP_

#include "message/Datatype.h"

namespace atom {
namespace message {
namespace types {

template<class T>
class BasicType : public Datatype
{
public:
	BasicType(xml::Node xmlNode) : Datatype(xmlNode)
	{
		this->myValue = 0; // Default value
	}

	virtual ~BasicType()
	{
	}

	T getValue()
	{
		return this->myValue;
	}

	void setValue(T value)
	{
		this->myValue = value;
	}

	void readBits(BitBuffer & buffer)
	{
		buffer.readBasicType(this->myLength, this->myValue);
	}

	void writeBits(BitBuffer & buffer)
	{
		buffer.writeBasicType(this->myLength, this->myValue);
	}


private:
	T myValue;
};

}
}
}

#endif // BASICTYPE_HPP_

