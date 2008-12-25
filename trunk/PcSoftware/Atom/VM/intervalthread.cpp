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

IntervalThread::IntervalThread(unsigned int timeout)
{
	myId = time(NULL);
	myTimeout = timeout;

	Thread<IntervalThread>();
}

void IntervalThread::run()
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	while (true)
	{
		usleep(myTimeout*1000);
		vm.queueExpression("Interval.triggerIntervalCallback(" + itos(myId) + ");");
	}
}
