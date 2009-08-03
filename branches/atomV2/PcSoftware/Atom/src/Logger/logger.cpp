/***************************************************************************
 *   Copyright (C) January 10, 2009 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   logger.cpp                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "logger.h"

Logger* Logger::myInstance = NULL;

Logger& Logger::getInstance()
{
	if (myInstance == NULL)
	{
		myInstance = new Logger();
	}

	return *myInstance;
}

void Logger::deleteInstance()
{
	if (myInstance != NULL)
	{
		delete myInstance;
		myInstance = NULL;
	}
}

Logger::Logger()
{
	openlog("Atom", LOG_ODELAY, LOG_DAEMON);
}

Logger::~Logger()
{
	if (myLogfile.is_open())
	{
		myLogfile << "\n\n";
		myLogfile.flush();
		myLogfile.close();
	}

	closelog();
}

bool Logger::open(string filename)
{
	myLogfile.open(filename.c_str(), ios::app);

	if (myLogfile.is_open())
	{
		myLogfile.close();
		return false;
	}

	return true;
}

void Logger::addToSyslog(string message)
{
	mySyslogMutex.lock();
	syslog(LOG_ODELAY, message.c_str());
	mySyslogMutex.unlock();
	add(message);
}

void Logger::add(string message)
{
	addRaw(formatMessage(message));
}

void Logger::addRaw(string message)
{
	myMutex.lock();

	cout << message;
	
	if (myLogfile.is_open())
	{
		myLogfile << message;
		myLogfile.flush();
	}

	myMutex.unlock();
}

string Logger::formatMessage(string message)
{
	if (message != "\n")
	{
		return "[" + niceTime() + "] " + message;
	}

	return message;
}
