/***************************************************************************
 *   Copyright (C) November 27, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   settings.h                                            *
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

#ifndef _SETTINGS_H
#define	_SETTINGS_H

using namespace std;

#include <string>
#include <map>
#include <fstream>

#include "../Tools/tools.h"
#include "../Logger/logger.h"
#include "../Threads/mutex.h"

///FIXME: Do we need to make this threadsafe... 

class Settings
{
public:
	static bool read(string filename);
	static void add(string name, string value);
	static string get(string name);
	
private:
	static string myFilename;
	static map<string, string> mySettings;
	static Mutex myMutex;
};

#endif	/* _SETTINGS_H */

