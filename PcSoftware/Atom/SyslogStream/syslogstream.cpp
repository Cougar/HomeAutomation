/***************************************************************************
 *   Copyright (C) November 27, 2008, 11:57 AM by Mattias Runge            *
 *   mattias@runge.se                                                      *
 *   syslogstream.cpp                                                      *
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

#include "syslogstream.h"

SyslogStream* SyslogStream::myInstance = NULL;

SyslogStream& SyslogStream::getInstance()
{
	if (myInstance == NULL)
	{
		myInstance = new SyslogStream();
	}

	return *myInstance;
}

void SyslogStream::deleteInstance()
{
	if (myInstance != NULL)
	{
		delete myInstance;
		myInstance = NULL;
	}
}

SyslogStream::SyslogStream()
{
	openlog("Atom", LOG_ODELAY, LOG_DAEMON);
}

SyslogStream::~SyslogStream()
{
	closelog();
}

void SyslogStream::add(string str)
{
	syslog(LOG_ODELAY, str.c_str());

	if (str == "\n")
	{
		cout << str;
	}
	else
	{
		struct tm tmStruct;
		time_t t = time(NULL);
		gmtime_r(&t, &tmStruct);

		cout	<< "["
				<< lpad(itos(tmStruct.tm_hour), 2, '0')
				<< ":"
				<< lpad(itos(tmStruct.tm_min), 2, '0')
				<< ":"
				<< lpad(itos(tmStruct.tm_sec), 2, '0')
				<< "] "
				<< str;
	}
}

SyslogStream& SyslogStream::operator <<(const string& str)
{
	add(str);
}

SyslogStream& SyslogStream::operator <<(const char* str)
{
	add(str);
}

SyslogStream& SyslogStream::operator <<(const int num)
{
	add(itos(num));
}


