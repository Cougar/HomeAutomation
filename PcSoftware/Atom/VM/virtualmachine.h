/***************************************************************************
 *   Copyright (C) December 10, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   virtualmachine.h                                            *
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

#ifndef _VIRTUALMACHINE_H
#define	_VIRTUALMACHINE_H

using namespace std;

#include <map>
#include <string>
#include <queue>

#include "../v8/include/v8.h"
#include "../SyslogStream/syslogstream.h"
#include "../Settings/settings.h"
#include "../CanNet/cannetmanager.h"
#include "../CanNet/canmessage.h"
#include "../Threads/semaphore.h"
#include "../Threads/threadsafequeue.h"
#include "intervalthread.h"
#include "socketthread.h"

using namespace v8;

Handle<Value> VirtualMachine_log(const Arguments& args);
Handle<Value> VirtualMachine_sendCanMessage(const Arguments& args);
Handle<Value> VirtualMachine_sendCanNMTMessage(const Arguments& args);
Handle<Value> VirtualMachine_loadScript(const Arguments& args);
Handle<Value> VirtualMachine_stopIntervalThread(const Arguments& args);
Handle<Value> VirtualMachine_startIntervalThread(const Arguments& args);
Handle<Value> VirtualMachine_stopSocketThread(const Arguments& args);
Handle<Value> VirtualMachine_startSocketThread(const Arguments& args);
Handle<Value> VirtualMachine_sendToSocketThread(const Arguments& args);
Handle<Value> VirtualMachine_uint2hex(const Arguments& args);

class VirtualMachine : public Thread<VirtualMachine>
{
public:
	static VirtualMachine& getInstance();
	static void deleteInstance();

	void queueCanMessage(CanMessage canMessage);
	void queueExpression(string expression);

	void run();

	bool loadScript(string scriptName);

	unsigned int startIntervalThread(unsigned int timeout);
	bool stopIntervalThread(unsigned int id);

	unsigned int startSocketThread(string address, int port, unsigned int reconnectTimeout);
	bool stopSocketThread(unsigned int id);
	void sendToSocketThread(unsigned int id, string data);

private:
	void callHandleNMTMessage(CanMessage canMessage);
	void callHandleMessage(CanMessage canMessage);
	bool runExpression(string expression);
	void printException(TryCatch* tryCatch);

	VirtualMachine();
	~VirtualMachine();
	static VirtualMachine* myInstance;

	Handle<Function> myFunctionHandleNMTMessage;
	Handle<Function> myFunctionOfflineCheck;
	Handle<Function> myFunctionHandleMessage;
	Handle<Function> myFunctionStartup;

	Handle<ObjectTemplate> myGlobal;
	HandleScope myHandleScope;
	Handle<Context> myContext;

	Semaphore mySemaphore;

	ThreadSafeQueue<string> myExpressions;
	ThreadSafeQueue<CanMessage> myCanMessages;

	map<int, IntervalThread> myIntervalThreads;
	map<int, SocketThread*> mySocketThreads;
};

#endif	/* _VIRTUALMACHINE_H */

