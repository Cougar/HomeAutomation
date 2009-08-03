/***************************************************************************
 *   Copyright (C) January 5, 2009 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   commandthread.cpp                                            *
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

#include "virtualmachine.h"
#include "commandthread.h"

CommandThread::CommandThread()
{
	myBuffer = "";
}

CommandThread::~CommandThread()
{
}

void CommandThread::handleClientData(int id, string data)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	myBuffer += data;
	
	string tempBuffer = str_replace("\r", "", myBuffer);
	tempBuffer = str_replace("\n", "\\n", tempBuffer);
	
	if (tempBuffer[tempBuffer.size()-1] == ';')
	{
		vm.queueExpression(Expression(id, tempBuffer));
		myBuffer = "";
	}
}
