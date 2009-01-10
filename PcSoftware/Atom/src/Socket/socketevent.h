/***************************************************************************
 *   Copyright (C) December 30, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   socketevent.h                                            *
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

#ifndef _SOCKETEVENT_H
#define	_SOCKETEVENT_H

using namespace std;

#include <string>

class SocketEvent
{
public:
	SocketEvent(unsigned int type, string data)
	{
		myType = type;
		myData = data;
	}

	SocketEvent(const SocketEvent& socketEvent)
	{
		myType = socketEvent.myType;
		myData = socketEvent.myData;
	}

	unsigned int getType() { return myType; };
	string getData() { return myData; };

	static const unsigned int TYPE_NONE					= 0;
	static const unsigned int TYPE_DATA					= 1;
	static const unsigned int TYPE_INACTIVITY			= 2;

	static const unsigned int TYPE_CONNECTING			= 3;
	static const unsigned int TYPE_CONNECTED			= 4;
	static const unsigned int TYPE_WAITING_RECONNECT	= 5;
	static const unsigned int TYPE_CONNECTION_CLOSED	= 6;
	static const unsigned int TYPE_CONNECTION_RESET		= 7;
	static const unsigned int TYPE_CONNECTION_DIED		= 8;
	static const unsigned int TYPE_CONNECTION_FAILED	= 9;

private:
	unsigned int myType;
	string myData;
};

#endif	/* _SOCKETEVENT_H */

