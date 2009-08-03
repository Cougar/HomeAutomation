/*
 * Database.cpp
 *
 *  Created on: Jul 23, 2009
 *      Author: Mattias Runge
 */

#include "Database.h"

namespace atom {
namespace db {

Database::pointer Database::Instance(new Database());

Database::Database()
{
	// TODO Auto-generated constructor stub

}

Database::~Database()
{
	// TODO Auto-generated destructor stub
}

Database::pointer Database::getInstance()
{
	return Database::Instance;
}

void Database::load(string filename)
{
	this->myXmlNode.load(filename);
}

xml::Node & Database::getRootNode()
{
	return this->myXmlNode;
}

}
}
