/***************************************************************************
 *   Copyright (C) November 27, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   settings.cpp                                            *
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

#include <string>


#include "settings.h"

string Settings::myFilename;
map<string, string> Settings::mySettings;

bool Settings::read(string filename)
{
	SyslogStream &slog = SyslogStream::getInstance();

	myFilename = filename;

	ifstream file(myFilename.c_str());

	if (!file)
	{
		slog << "Could not find settings file: " + myFilename + ".\n";
		return false;
	}

	slog << "Loading settings from " + filename + ".\n";

	while (!file.eof())
	{
		string line;
		getline(file, line);

		trim(line);

		if (line.length() == 0)
			continue;

		vector<string> parts = explode("=", line);

		if (parts.size() < 2)
			continue;

		add(trim(parts[0]), trim(parts[1]));
	}

	file.close();

	return true;
}

void Settings::add(string name, string value)
{
	mySettings[name] = value;
}

string Settings::get(string name)
{
	map<string, string>::const_iterator iterator = mySettings.find(name);

	if (iterator == mySettings.end())
		return NULL;

	return iterator->second;
}
