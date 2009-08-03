/*
 * Datatype.h
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#ifndef DATATYPE_H_
#define DATATYPE_H_

#include <exception>
#include <boost/make_shared.hpp>
#include <boost/any.hpp>
#include "../utils/Logger.h"
#include "types/Selection.h"
#include "../utils/BitBuffer.h"

namespace atom {
namespace message {

using namespace atom::utils;
using namespace atom::message;

enum datatypes {
	DATATYPE_UNSIGNED_INTEGER,
	DATATYPE_INTEGER,
	DATATYPE_DECIMAL,
	DATATYPE_BOOLEAN,
	DATATYPE_STRING,
	DATATYPE_ENUM,
	DATATYPE_RESPONSE,
	DATATYPE_UNKNOWN
};

class Datatype
{
public:
	typedef boost::shared_ptr<Datatype> pointer;

	Datatype(datatypes type);
	virtual ~Datatype();

	boost::any getValue();
	void setValue(boost::any value);

	void readBits(BitBuffer & buffer, unsigned int length);

	datatypes getType();

	static datatypes getTypeFromString(string name);

private:
	Logger LOG;

	datatypes myType;
	boost::any myValue;
};

}
}

#endif /* DATATYPE_H_ */
