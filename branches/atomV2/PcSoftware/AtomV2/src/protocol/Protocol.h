/*
 * Protocol.h
 *
 *  Created on: Jul 23, 2009
 *      Author: Mattias Runge
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <string>
#include <boost/make_shared.hpp>
#include "xml/Node.h"
#include "utils/Logger.h"

namespace atom {

using namespace std;
using namespace utils;

class Protocol
{
public:
	typedef boost::shared_ptr<Protocol> pointer;

	virtual	~Protocol();

	static pointer getInstance();

	void load(string filename);

	xml::Node & getRootNode();

private:
	Protocol();

	static pointer Instance;

	Logger LOG;

	xml::Node myXmlNode;
};

}

#endif /* PROTOCOL_H_ */
