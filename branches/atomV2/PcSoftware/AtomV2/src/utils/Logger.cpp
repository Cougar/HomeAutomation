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
	this->myClassName = "UnknownClass";
}

Logger::~Logger()
{
}

void Logger::setName(string className)
{
	this->myClassName = className;
}

string Logger::getFormatedTime()
{
	struct tm tmStruct;
	time_t t = time(NULL);
	localtime_r(&t, &tmStruct);

	string hours = convert::int2string(tmStruct.tm_hour);
	string minutes = convert::int2string(tmStruct.tm_min);
	string seconds = convert::int2string(tmStruct.tm_sec);

	string_::pad_left(hours, 2, '0');
	string_::pad_left(minutes, 2, '0');
	string_::pad_left(seconds, 2, '0');

	return hours + ":" + minutes + ":" + seconds;
}

void Logger::info(string message)
{
	cout << Logger::getFormatedTime() << " INFO  " << string_::pad_right_copy(this->myClassName, 20, ' ') << message << endl;
}

void Logger::debug(string message)
{
	cout << Logger::getFormatedTime() << " DEBUG " << string_::pad_right_copy(this->myClassName, 20, ' ') << message << endl;
}

void Logger::warn(string message)
{
	cout << Logger::getFormatedTime() << " WARN  " << string_::pad_right_copy(this->myClassName, 20, ' ') << message << endl;
}

void Logger::error(string message)
{
	cout << Logger::getFormatedTime() << " ERROR " << string_::pad_right_copy(this->myClassName, 20, ' ') << message << endl;
}

void Logger::fatal(string message)
{
	cout << Logger::getFormatedTime() << " FATAL " << string_::pad_right_copy(this->myClassName, 20, ' ') << message << endl;
}


}
}
