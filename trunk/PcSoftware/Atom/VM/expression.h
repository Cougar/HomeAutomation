/***************************************************************************
 *   Copyright (C) January 5, 2009 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   expression.h                                            *
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

#ifndef _EXPRESSION_H
#define	_EXPRESSION_H

using namespace std;

#include <string>

class Expression
{
public:
	Expression(string script)
	{
		myTargetId = 0;
		myScript = script;
	};

	Expression(unsigned int targetId, string script)
	{
		myTargetId = targetId;
		myScript = script;
	};

	~Expression() { };

	unsigned int getTargetId() { return myTargetId; };
	string getScript() { return myScript; };

private:
	string myScript;
	unsigned int myTargetId;
};

#endif	/* _EXPRESSION_H */

