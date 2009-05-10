/***************************************************************************
 *   Copyright (C) December 26, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   socketthread.h                                            *
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

#ifndef _SOCKETTHREAD_H
#define	_SOCKETTHREAD_H

using namespace std;

#include <string>
#include <time.h>
#include <stdexcept>

#include "../Socket/asyncsocket.h"
#include "../Threads/thread.h"
#include "../Tools/tools.h"
#include "../Threads/mutex.h"

class SocketThread : public Thread<SocketThread>
{
public:
	SocketThread() { Thread<SocketThread>(); };
	SocketThread(string address, unsigned int port, unsigned int reconnectTimeout);
	~SocketThread();

	unsigned long int getId() { return myId; };
	void send(string data);
	
	void startSocket();

	void run();

private:
	unsigned long int myId;
	AsyncSocket *mySocket;
	
	static unsigned long int myCount;
	static Mutex myCountMutex;

};

#endif	/* _SOCKETTHREAD_H */

