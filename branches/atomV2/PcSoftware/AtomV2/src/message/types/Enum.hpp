/*
 * Enum.hpp
 *
 *  Created on: Aug 05, 2009
 *      Author: Mattias Runge
 */

#ifndef ENUM_HPP_
#define ENUM_HPP_

#include "message/Datatype.h"
#include "utils/convert.h"

namespace atom {
namespace message {
namespace types {

class Enum : public Datatype
{
public:
	Enum(xml::Node xmlNode) : Datatype(xmlNode)
	{
		xml::Node::nodeList labelNodes = xmlNode.select("label");

		if (labelNodes.size() > 0)
		{
			for (unsigned int n = 0; n < labelNodes.size(); n++)
			{
				this->myValues.push_back(labelNodes[n]["name"]);
			}

			this->myValue = this->myValues[0]; // Default value is the first defined

			this->myLength = convert::stateCount2bitCount(this->myValues.size());
		}
		else
		{
			// TODO Print warning about empty enum...
			this->myValue = ""; // Default value
		}
	}

	virtual ~Enum()
	{
	}

	string_list getValues()
	{
		return this->myValues;
	}

	string getValue()
	{
		return this->myValue;
	}

	void setValue(string value)
	{
		// TODO Check for validity

		this->myValue = value;
	}

	void readBits(BitBuffer & buffer)
	{
		unsigned long index = 0;
		buffer.read(this->myLength, index);

		// TODO Check index for validity

		this->myValue = this->myValues[index];
	}

	void writeBits(BitBuffer & buffer)
	{
		unsigned long index = 0;

		for (unsigned int n = 0; n < this->myValues.size(); n++)
		{
			if (this->myValues[n] == this->myValue)
			{
				index = n;
				break;
			}
		}

		buffer.write(this->myLength, index);
	}


	string toString()
	{
		return "\"" + this->myValue + "\"";
	}


private:
	string myValue;
	string_list myValues;
};

}
}
}

#endif // ENUM_HPP_

