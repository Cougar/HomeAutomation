/***************************************************************************
 *   Copyright (C) January 10, 2009 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   logger.h                                            *
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

#ifndef _LOGGER_H
#define	_LOGGER_H

using namespace std;

#include <stddef.h>
#include <syslog.h>
#include <iostream>
#include "../Tools/tools.h"
#include "../Threads/mutex.h"

class Logger
{
public:
	static Logger& getInstance();
	static void deleteInstance();

	bool open(string filename);

	void addToSyslog(string message);
	void add(string message);

protected:
	Logger();
	~Logger();

	void addRaw(string message);

	string formatMessage(string message);

private:
	static Logger* myInstance;

	Mutex mySyslogMutex;
	Mutex myMutex;
	ofstream myLogfile;
};

#endif	/* _LOGGER_H */

