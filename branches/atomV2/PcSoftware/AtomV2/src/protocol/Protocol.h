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
#include "log/Logger.h"

namespace atom {

using namespace std;

class Protocol
{
public:
	typedef boost::shared_ptr<Protocol> pointer;

	virtual	~Protocol();

	static pointer getInstance();

	void load(string filename, string cachefilename);

	xml::Node & getRootNode();
	xml::Node & getRootCacheNode();

	bool isLoaded();

private:
	Protocol();

	static pointer Instance;

	log::Logger LOG;

	bool myIsLoaded;
	xml::Node myXmlNode;
	xml::Node myXmlCacheNode;
};

}

#endif /* PROTOCOL_H_ */
