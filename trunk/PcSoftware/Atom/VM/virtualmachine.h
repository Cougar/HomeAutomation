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
#include "commandthread.h"
#include "expression.h"

using namespace v8;

class VirtualMachine : public Thread<VirtualMachine>
{
public:
	static VirtualMachine& getInstance();
	static void deleteInstance();

	void run();

	void queueExpression(Expression expression);
	void queueExpression(string script) { queueExpression(Expression(script)); };

	bool loadScript(string scriptName);
	bool loadDataStore(string storeName);

	unsigned int startIntervalThread(unsigned int timeout);
	bool stopIntervalThread(unsigned int id);

	unsigned int startSocketThread(string address, int port, unsigned int reconnectTimeout);
	bool stopSocketThread(unsigned int id);
	void sendToSocketThread(unsigned int id, string data);

	void sendToCommandClient(unsigned int id, string data);
	void disconnectCommandClient(unsigned int id);

	void print(unsigned int id, string data);

	static Handle<Value> _quit(const Arguments& args);
	static Handle<Value> _print(const Arguments& args);
	static Handle<Value> _log(const Arguments& args);
	static Handle<Value> _sendCanMessage(const Arguments& args);
	static Handle<Value> _sendCanNMTMessage(const Arguments& args);
	static Handle<Value> _loadScript(const Arguments& args);
	static Handle<Value> _stopIntervalThread(const Arguments& args);
	static Handle<Value> _startIntervalThread(const Arguments& args);
	static Handle<Value> _stopSocketThread(const Arguments& args);
	static Handle<Value> _startSocketThread(const Arguments& args);
	static Handle<Value> _sendToSocketThread(const Arguments& args);

	static Handle<Value> _uint2hex(const Arguments& args);
	static Handle<Value> _hex2bin(const Arguments& args);
	static Handle<Value> _bin2hex(const Arguments& args);
	static Handle<Value> _bin2float(const Arguments& args);
	static Handle<Value> _float2bin(const Arguments& args);
	static Handle<Value> _bin2uint(const Arguments& args);
	static Handle<Value> _uint2bin(const Arguments& args);
	static Handle<Value> _hex2uint(const Arguments& args);


	static Handle<Value> _loadDataStore(const Arguments& args);
	static Handle<Value> _getFileContents(const Arguments& args);

private:
	bool runExpression(Expression expression);
	string formatException(TryCatch* tryCatch);

	VirtualMachine();
	~VirtualMachine();
	static VirtualMachine* myInstance;

	Handle<Function> myFunctionStartup;

	Handle<ObjectTemplate> myGlobal;
	HandleScope myHandleScope;
	Handle<Context> myContext;

	Semaphore mySemaphore;

	ThreadSafeQueue<Expression> myExpressions;

	map<int, IntervalThread*> myIntervalThreads;
	map<int, SocketThread*> mySocketThreads;

	CommandThread myCommandThread;
};

#endif	/* _VIRTUALMACHINE_H */

