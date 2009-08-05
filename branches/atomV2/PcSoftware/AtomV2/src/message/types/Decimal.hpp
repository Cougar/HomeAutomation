/*
 * Decimal.hpp
 *
 *  Created on: Aug 05, 2009
 *      Author: Mattias Runge
 */

#ifndef DECIMAL_HPP_
#define DECIMAL_HPP_

#include "message/Datatype.h"
#include "utils/convert.h"

namespace atom {
namespace message {
namespace types {

class Decimal : public Datatype
{
public:
	Decimal(xml::Node xmlNode) : Datatype(xmlNode)
	{
		this->myValue = 0; // Default value
		this->myScaling = convert::string2uint(xmlNode["scaling"]);

		// We do not want to scale with 0 because we will divide by the scaling later
		if (this->myScaling == 0)
		{
			this->myScaling = 1;
		}
	}

	virtual ~Decimal()
	{
	}

	double getValue()
	{
		return this->myValue;
	}

	void setValue(double value)
	{
		this->myValue = value;
	}

	void readBits(BitBuffer & buffer)
	{
		long value = 0;
		buffer.readBasicType(this->myLength, value);
		this->myValue = (double)value / this->myScaling; // TODO check this for correctness
	}

	void writeBits(BitBuffer & buffer)
	{
		long value = (long) (this->myValue * this->myScaling); // TODO check this for correctness
		buffer.writeBasicType(this->myLength, value);
	}


private:
	double myValue;
	unsigned int myScaling;
};

}
}
}

#endif // DECIMAL_HPP_

