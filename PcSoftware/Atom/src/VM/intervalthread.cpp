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

#include "intervalthread.h"
#include "virtualmachine.h"

unsigned int IntervalThread::myCount = 0;
Mutex IntervalThread::myCountMutex;

IntervalThread::IntervalThread(unsigned int timeout)
{
	myCountMutex.lock();
	myId = myCount++;
	myCountMutex.unlock();

	myTimeout = timeout;

	Thread<IntervalThread>();
}

void IntervalThread::run()
{
	VirtualMachine &vm = VirtualMachine::getInstance();
	myReset = false;
	bool localReset = myReset;

	while (true)
	{
		usleep(getTimeout()*1000);

		myResetMutex.lock();
		localReset = myReset;
		myReset = false;
		myResetMutex.unlock();
		
		if (!localReset)
		{
			vm.queueExpression("Interval.triggerIntervalCallback(" + itos(myId) + ");");
		}
	}
}

unsigned int IntervalThread::getTimeout()
{
	return myTimeout;
}

void IntervalThread::setTimeout(unsigned int timeout)
{
	if (myTimeout == timeout)
	{
		myResetMutex.lock();
		myReset = true;
		myResetMutex.unlock();
	}
	else
	{
		stop();
		myTimeout = timeout;
		start();
	}
}
