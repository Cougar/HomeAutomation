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

#ifndef _SYSLOGSTREAM_H
#define	_SYSLOGSTREAM_H

using namespace std;

#include <stddef.h>
#include <syslog.h>
#include <ostream>
#include <iostream>
#include <time.h>
#include "../Tools/tools.h"

class SyslogStream : public ostream
{
public:
	static SyslogStream& getInstance();
	static void deleteInstance();

	void add(string str);

	SyslogStream& operator <<(const string& str);
	SyslogStream& operator <<(const char* str);
	SyslogStream& operator <<(const int num);
	
protected:
	SyslogStream();
	~SyslogStream();

private:
	static SyslogStream* myInstance;
};

#endif	/* _SYSLOGSTREAM_H */

