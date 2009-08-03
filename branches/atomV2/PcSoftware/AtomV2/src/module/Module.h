/*
 * Module.h
 *
 *  Created on: May 9, 2009
 *      Author: Mattias Runge
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <string>

namespace atom {
namespace db {

using namespace std;

class Module
{
public:
	Module(string name);
	virtual ~Module();

protected:
	string name;
	unsigned short id;
};

}
}

#endif /* MODULE_H_ */
