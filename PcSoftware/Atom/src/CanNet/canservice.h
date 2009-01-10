/***************************************************************************
 *   Copyright (C) December 4, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   canservice.h                                            *
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

#ifndef _CANSERVICE_H
#define	_CANSERVICE_H

using namespace std;

#include <string>

#include <time.h>

#include "../Services/service.h"
#include "canmessage.h"

class CanService : public Service
{
public:
	CanService(string type, string name, unsigned int id);
	~CanService();

	unsigned int getHardwareId() { return myHardwareId; };
	void setHardwareId(unsigned int hardwareId) { myHardwareId = hardwareId; };

	bool isOnline() { return myLastOnline+10 > time(NULL); };
	unsigned int getLastOnline() { return myLastOnline; };
	void registerActivity() { myLastOnline = time(NULL); };

	void handleCanMessage(string message);
	string toString();

private:
	unsigned int myHardwareId;
	unsigned int myLastOnline;
};

#endif	/* _CANSERVICE_H */

