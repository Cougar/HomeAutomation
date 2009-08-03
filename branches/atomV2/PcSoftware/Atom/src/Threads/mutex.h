/***************************************************************************
 *   Copyright (C) November 29, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   Mutex.h                                            *
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

#ifndef _MUTEX_H
#define	_MUTEX_H

#include <pthread.h>

class Mutex
{
public:
	Mutex() { pthread_mutex_init(&myMutex, NULL); };
	~Mutex() { pthread_mutex_destroy(&myMutex); };

	int tryToLock() { return pthread_mutex_trylock(&myMutex); };
	int lock() { return pthread_mutex_lock(&myMutex); };
	int unlock() { return pthread_mutex_unlock(&myMutex); };

protected:
	pthread_mutex_t myMutex;
};


#endif	/* _MUTEX_H */

