/***************************************************************************
 *   Copyright (C) November 29, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   thread.h                                            *
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

#ifndef _THREAD_H
#define	_THREAD_H

#include <pthread.h>
#include <iostream>

template <class T>
class Thread
{
public:
	Thread()
	{
		myIsCreated = false;
		myError = 0;
	};
	virtual ~Thread()
	{
		stop();
	};

	bool stop()
	{

		if (!myIsCreated)
		{
			//cout << "Thread is not running, can not be stopped " << endl;
			return true;
		}

		//cout << "Thread cancel to run " << endl;

		myError = pthread_cancel(myHandle);
		//myError = pthread_detach(myHandle);
		//cout << "Thread cancel returned " << myError << endl;



		//if (joinThread)
		{
			join();
		}

		myIsCreated = false;

		return myError == 0;
	};

	bool start()
	{
		myError = pthread_create(&myHandle, NULL, T::doThread, (void*)this);

		if (myError != 0)
		{
			myIsCreated = false;
			return false;
		}

		myIsCreated = true;
		return true;
	};

	bool join()
	{
		if (!myIsCreated)
			return false;

		myError = pthread_join(myHandle, NULL);

		if (myError != 0)
		{
			myIsCreated = false;
			return false;
		}

		myIsCreated = true;
		return true;
	};

	int getError()
	{
		return myError;
	};

	static void *doThread(void* ptr)
	{
		((T*)ptr)->run();
	};

	virtual void run() { };

private:
	bool myIsCreated;
	pthread_t myHandle;
	int myError;
};

#endif	/* _THREAD_H */

