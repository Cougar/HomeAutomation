/***************************************************************************
 *   Copyright (C) December 11, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   offlinecheck.h                                            *
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

#ifndef _INTERVALTHREAD_H
#define	_INTERVALTHREAD_H

using namespace std;

#include <string>
#include <time.h>

#include "../Threads/thread.h"
#include "../Tools/tools.h"
#include "../Threads/mutex.h"

class IntervalThread : public Thread<IntervalThread>
{
public:
	IntervalThread() { Thread<IntervalThread>(); };
	IntervalThread(unsigned int timeout);
	~IntervalThread() { stop(); };

	unsigned int getId() { return myId; };

	unsigned int getTimeout();
	void setTimeout(unsigned int timeout);

	void run();

private:
	unsigned int myId;
	unsigned int myTimeout;

	bool myReset;
	Mutex myResetMutex;

	static unsigned int myCount;
	static Mutex myCountMutex;
};

#endif	/* _INTERVALTHREAD_H */

