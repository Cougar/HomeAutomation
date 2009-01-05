/***************************************************************************
 *   Copyright (C) January 4, 2009 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   server.h                                            *
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

#ifndef _SERVER_H
#define	_SERVER_H

using namespace std;

#include <string>
#include <map>

#include "../Threads/thread.h"
#include "asyncsocket.h"
#include "socketexception.h"
#include "socketeventcallback.h"
#include "../version.h"
#include "../SyslogStream/syslogstream.h"

class Server : public Thread<Server>, public SocketEventCallback
{
public:
	Server() { };
	Server(int port, string name);
	~Server();

	void setSettings(int port, string name);
	void sendToAll(string data);
	void sendTo(int id, string data);
	void disconnectClient(int id);

	void run();
	void handleEvent(int id, SocketEvent socketEvent);

protected:
	virtual void handleClientData(int id, string data);

	string myName;
	
	AsyncSocket mySocket;
	map<int, AsyncSocket*> myClientSockets;
};


#endif	/* _SERVER_H */

