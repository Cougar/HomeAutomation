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
#include "Datatype.h"
#include "../utils/BitBuffer.h"

namespace atom {
namespace message {

using namespace std;
using namespace atom::utils;

class Variable
{
public:
	Variable();
	Variable(string datatype, bool required, string unit);
	virtual ~Variable();

	void readBits(BitBuffer & buffer);

private:
	boost::shared_ptr<Datatype> myDatatype;
	bool myRequired;
	string myUnit;
	unsigned int myLength;
};

}
}

#endif /* VARIABLE_H_ */
