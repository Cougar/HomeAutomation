/***************************************************************************
 *   Copyright (C) November 26, 2008, 3:00 PM by Mattias Runge             *
 *   mattias@runge.se                                                      *
 *   main.cpp                                                              *
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

using namespace std;

#include <stdlib.h>
#include <iostream>
#include <signal.h>

#include "version.h"
#include "Tools/tools.h"
#include "Logger/logger.h"
#include "Settings/settings.h"
#include "Socket/server.h"
#include "CanNet/cannetmanager.h"
#include "Socket/asyncsocket.h"
#include "VM/virtualmachine.h"
#include "CanNet/candebug.h"

bool cleanUpRunning = false;
int cleanUp();
void handler(int status);

int main(int argc, char** argv)
{
	signal(SIGTERM, handler);
	signal(SIGINT, handler);
	signal(SIGQUIT, handler);
	signal(SIGABRT, handler);
	signal(SIGPIPE, handler);

	Logger &log = Logger::getInstance();

	log.addToSyslog("Atom " + ftos(VERSION) + " starting...\n");
	log.add("Written by Mattias Runge 2008-2009\n\n");

	if (file_exists("/etc/atom.conf"))
	{
		if (!Settings::read("/etc/atom.conf"))
		{
			return cleanUp();
		}
	}
	else
	{
		if (!Settings::read("atom.conf"))
		{
			return cleanUp();
		}
	}

	string logfile = Settings::get("Logfile");
	if (logfile != "")
	{
		if (!log.open(logfile))
		{
			log.addToSyslog("Could not open logfile, " + logfile + "\n");
		}
		else
		{
			log.add("Logging to " + logfile + "\n");
		}
	}
	else
	{
		log.add("Logfile is not defined, logging will be done to syslog and console only\n");
	}

	VirtualMachine &vm = VirtualMachine::getInstance();
	vm.start();

	string canDebugPort = Settings::get("CanDebugPort");
	if (canDebugPort != "")
	{
		CanDebug &canDebug = CanDebug::getInstance();
		canDebug.setSettings(stoi(canDebugPort), "CanDebug");
		canDebug.start();
	}
	else
	{
		log.add("CanDebugPort is not defined, can not start CanDebug socket\n");
	}

	CanNetManager &canMan = CanNetManager::getInstance();
	canMan.start();

	if (Settings::get("DaemonMode") == "yes")
	{
		log.add("Entering daemon mode...\n");
		if (daemon(0, 0) == -1)
		{
			log.addToSyslog("Could not enter daemon mode. Exiting...\n");
			return cleanUp();
		}
	}

	vm.join();

	return cleanUp();
}

int cleanUp()
{
	Logger &log = Logger::getInstance();

	if (!cleanUpRunning)
	{
		cleanUpRunning = true;

		log.add("\n");
		log.addToSyslog("Thank you for using Atom. Goodbye!\n");

		CanDebug::deleteInstance();
		VirtualMachine::deleteInstance();
		CanNetManager::deleteInstance();
		Logger::deleteInstance();
	}
	else
	{
		log.addToSyslog("Something has gone wrong clean up is already running!\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void handler(int status)
{
	string signalName = "Unknown";

	switch (status)
	{
	case SIGTERM:
	signalName = "Terminate";
	break;

	case SIGINT:
	signalName = "Interupt";
	break;

	case SIGQUIT:
	signalName = "Quit";
	break;

	case SIGABRT:
	signalName = "Abort";
	break;

	case SIGIO:
	signalName = "I/O";
	break;

	case SIGPIPE:
	signalName = "Pipe";
	break;
	}

	Logger &log = Logger::getInstance();
	log.addToSyslog("Received signal " + signalName + "(" + itos(status) + ")\n");

	if (status == SIGPIPE)
	{
		return;
	}

	exit(cleanUp());
}
