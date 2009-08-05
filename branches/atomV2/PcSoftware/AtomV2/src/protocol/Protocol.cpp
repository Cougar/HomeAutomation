/*
 * Protocol.cpp
 *
 *  Created on: Jul 23, 2009
 *      Author: Mattias Runge
 */

#include "Protocol.h"

namespace atom {

Protocol::pointer Protocol::Instance(new Protocol());

Protocol::Protocol()
{
	LOG.setName("Protocol");
}

Protocol::~Protocol()
{
}

Protocol::pointer Protocol::getInstance()
{
	return Protocol::Instance;
}

void Protocol::load(string filename, string cachefilename)
{
	this->myXmlNode.load(filename);

	LOG.info("Loaded protocol from \"" + filename + "\".");

	// TODO Catch exceptions thrown by load

	// TODO Do validation of XML-data, check if needed sections are defined


	this->myXmlCacheNode.load(cachefilename);

	LOG.info("Loaded protocol cache from \"" + cachefilename + "\".");

	// TODO If not found generate one

	// TODO Check if versions match, if not update cache
}

xml::Node & Protocol::getRootNode()
{
	return this->myXmlNode;
}

xml::Node & Protocol::getRootCacheNode()
{
	return this->myXmlCacheNode;
}

}

