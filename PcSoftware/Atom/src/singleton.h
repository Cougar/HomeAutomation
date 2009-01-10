/***************************************************************************
 *   Copyright (C) January 5, 2009 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   singleton.h                                            *
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

#ifndef _SINGLETON_H
#define	_SINGLETON_H

#include <stdlib.h>

template <class T>
class Singleton
{
public:
	static T& getInstance()
	{
		if (myInstance == NULL)
		{
			myInstance = new T();
		}

		return *myInstance;
	}

	static void deleteInstance()
	{
		if (myInstance != NULL)
		{
			delete myInstance;
			myInstance = NULL;
		}
	}

protected:
	Singleton();
	~Singleton();
	static T* myInstance;
};

#endif	/* _SINGLETON_H */

