/*
 * Variable.h
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#ifndef VARIABLE_H_
#define VARIABLE_H_

#include <string>
#include <boost/make_shared.hpp>
#include "message/Datatype.h"
#include "message/BitBuffer.h"
#include "protocol/Protocol.h"
#include "utils/convert.h"
#include "xml/Node.h"

namespace atom {
namespace message {

using namespace std;

class Variable
{
public:
	Variable();
	Variable(xml::Node xmlNode);
	virtual ~Variable();

	string getName();
	Datatype::pointer getDatatype();
	string getUnit();
	bool isRequired();

	void readBits(BitBuffer & buffer);
	void writeBits(BitBuffer & buffer);

	string toString();

private:
	string myName;
	Datatype::pointer myDatatype;
	bool myRequired;
	string myUnit;
};

}
}

#endif /* VARIABLE_H_ */
