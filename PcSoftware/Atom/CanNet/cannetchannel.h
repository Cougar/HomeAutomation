/***************************************************************************
 *   Copyright (C) November 27, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   cannetchannel.h                                            *
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

#ifndef _CANNETCHANNEL_H
#define	_CANNETCHANNEL_H

using namespace std;

#include <string>

#include "cannetmanager.h"
#include "canmessage.h"
#include "../Socket/ClientSocket.h"
#include "../Socket/SocketException.h"
#include "../Settings/settings.h"
#include "../Threads/thread.h"

class CanNetChannel : public Thread<CanNetChannel>
{
public:
	CanNetChannel();
	virtual ~CanNetChannel();
	
	void run();
	void sendString(string data);

private:
	ClientSocket myClientSocket;
};

#endif	/* _CANNETCHANNEL_H */

