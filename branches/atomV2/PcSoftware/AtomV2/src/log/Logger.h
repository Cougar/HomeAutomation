/*
 * Logger.h
 *
 *  Created on: Jul 18, 2009
 *      Author: Mattias Runge
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <iostream>
#include "utils/convert.h"
#include "utils/string.h"

namespace atom {
namespace log {

using namespace std;

class Logger
{
public:
	Logger();
	virtual ~Logger();

	void setName(string className);

	static string getFormatedTime();

	void info(string message);
	void debug(string message);
	void warn(string message);
	void error(string message);
	void fatal(string message);

private:
	string myClassName;
};

}
}

#endif /* LOGGER_H_ */
