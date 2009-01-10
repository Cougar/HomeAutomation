/***************************************************************************
 *   Copyright (C) November 29, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   semaphore.h                                            *
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

#ifndef _SEMAPHORE_H
#define	_SEMAPHORE_H

#include "mutex.h"
#include <sys/time.h>

class Semaphore : public Mutex
{
public:
	Semaphore() { pthread_cond_init(&myCondition, NULL); };
	~Semaphore() { pthread_cond_destroy(&myCondition); };

	int broadcast() { return pthread_cond_broadcast(&myCondition); };
	int wait() { return pthread_cond_wait(&myCondition, &myMutex); };
	int wait(int timeout) // Returns ETIMEDOUT on timeout
	{
		struct timeval timeval;

		gettimeofday(&timeval, NULL);

		 /* Convert from timeval to timespec */
		myTimespec.tv_sec  = timeval.tv_sec;
		myTimespec.tv_nsec = timeval.tv_usec * 1000;
		myTimespec.tv_sec += timeout;

		return pthread_cond_timedwait(&myCondition, &myMutex, &myTimespec);
	}

protected:
	pthread_cond_t myCondition;
	struct timespec myTimespec;
	
};


#endif	/* _SEMAPHORE_H */

