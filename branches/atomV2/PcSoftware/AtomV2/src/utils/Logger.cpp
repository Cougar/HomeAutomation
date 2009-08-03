/*
 * Logger.cpp
 *
 *  Created on: Jul 18, 2009
 *      Author: Mattias Runge
 */

#include "Logger.h"

namespace atom {
namespace utils {

Logger::Logger()
{
	this->_className = "UnknownClass";
}

Logger::~Logger()
{
}

void Logger::setName(string className)
{
	this->_className = className;
}

void Logger::info(string message)
{
	cout << niceTime() << " INFO  " << rpad(this->_className, 20, ' ') << message << endl;
}

void Logger::debug(string message)
{
	cout << niceTime() << " DEBUG " << rpad(this->_className, 20, ' ') << message << endl;
}

void Logger::warn(string message)
{
	cout << niceTime() << " WARN  " << rpad(this->_className, 20, ' ') << message << endl;
}

void Logger::error(string message)
{
	cout << niceTime() << " ERROR " << rpad(this->_className, 20, ' ') << message << endl;
}

void Logger::fatal(string message)
{
	cout << niceTime() << " FATAL " << rpad(this->_className, 20, ' ') << message << endl;
}


}
}
