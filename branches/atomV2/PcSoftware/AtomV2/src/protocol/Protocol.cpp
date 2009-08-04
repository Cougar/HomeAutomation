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
	// TODO Auto-generated constructor stub

}

Protocol::~Protocol()
{
	// TODO Auto-generated destructor stub
}

Protocol::pointer Protocol::getInstance()
{
	return Protocol::Instance;
}

void Protocol::load(string filename)
{
	this->myXmlNode.load(filename);

	// TODO Catch exceptions thrown by load

	// TODO Do validation of XML-data, check if needed sections are defined
}

xml::Node & Protocol::getRootNode()
{
	return this->myXmlNode;
}

}

