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
#include "utils/BitBuffer.h"
#include "db/Database.h"

namespace atom {
namespace message {

using namespace std;
using namespace utils;

class Variable
{
public:
	Variable();
	Variable(string name, string datatype, bool required, string unit);
	virtual ~Variable();

	boost::any getValue();
	void setValue(boost::any value);

	void readBits(BitBuffer & buffer);

private:
	string myName;
	Datatype::pointer myDatatype;
	bool myRequired;
	string myUnit;
	unsigned int myLength;
};

}
}

#endif /* VARIABLE_H_ */
