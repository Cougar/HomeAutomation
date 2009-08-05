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
#include "log/Logger.h"
#include "message/BitBuffer.h"
#include "protocol/Protocol.h"


namespace atom {
namespace message {

class Datatype
{
public:
	typedef boost::shared_ptr<Datatype> pointer;

	Datatype(xml::Node xmlNode);
	virtual ~Datatype();

	virtual void readBits(BitBuffer & buffer);
	virtual void writeBits(BitBuffer & buffer);

	virtual string toString();

	static pointer create(string name);

protected:
	string myType;
	string myName;
	unsigned int myLength;

private:
	log::Logger LOG;
};

}
}

#endif /* DATATYPE_H_ */
