/*
 * Database.h
 *
 *  Created on: Jul 23, 2009
 *      Author: Mattias Runge
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include <boost/make_shared.hpp>
#include "xml/Node.h"
#include "utils/Logger.h"

namespace atom {
namespace db {

using namespace std;
using namespace utils;

class Database
{
public:
	typedef boost::shared_ptr<Database> pointer;

	virtual	~Database();

	static pointer getInstance();

	void load(string filename);

	xml::Node & getRootNode();

private:
	Database();

	static pointer Instance;

	Logger LOG;

	xml::Node myXmlNode;
};

}
}

#endif /* DATABASE_H_ */
