/***************************************************************************
 *   Copyright (C) January 5, 2009 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   commandthread.h                                            *
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

#ifndef _COMMANDTHREAD_H
#define	_COMMANDTHREAD_H

using namespace std;

#include <string>

#include "../Socket/server.h"


class CommandThread : public Server
{
public:
	CommandThread();
	~CommandThread();

protected:
	void handleClientData(int id, string data);

private:

};

#endif	/* _COMMANDTHREAD_H */

