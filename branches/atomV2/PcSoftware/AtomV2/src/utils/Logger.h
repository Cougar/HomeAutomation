/*
 * Logger.h
 *
 *  Created on: Jul 18, 2009
 *      Author: Mattias Runge
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include "Utils.h"
#include <iostream>

namespace atom {
namespace utils {

using namespace std;

class Logger
{
public:
	Logger();
	virtual ~Logger();

	void setName(string className);

	void info(string message);
	void debug(string message);
	void warn(string message);
	void error(string message);
	void fatal(string message);

private:
	string _className;
};

}
}

#endif /* LOGGER_H_ */
