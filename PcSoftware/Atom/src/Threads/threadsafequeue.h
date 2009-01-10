/***************************************************************************
 *   Copyright (C) December 20, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   threadsafequeue.h                                            *
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

#ifndef _THREADSAFEQUEUE_H
#define	_THREADSAFEQUEUE_H

using namespace std;

#include <queue>
#include "mutex.h"

template <class T>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue() {};
	~ThreadSafeQueue() {};
	
	void push(T item)
	{
		myMutex.lock();
		myQueue.push(item);
		myMutex.unlock();
	}
	
	T pop()
	{
		myMutex.lock();
		T item = myQueue.front();
		myQueue.pop();
		myMutex.unlock();
		return item;
	}
	
	int size()
	{
		myMutex.lock();
		int size = myQueue.size();
		myMutex.unlock();
		return size;
	}

private:
	queue<T> myQueue;
	Mutex myMutex;
};

#endif	/* _THREADSAFEQUEUE_H */

