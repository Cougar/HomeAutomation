/***************************************************************************
 *   Copyright (C) December 10, 2008 by Mattias Runge                             *
 *   mattias@runge.se                                                      *
 *   virtualmachine.cpp                                            *
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

#include <map>


#include "virtualmachine.h"

VirtualMachine* VirtualMachine::myInstance = NULL;

VirtualMachine& VirtualMachine::getInstance()
{
	if (myInstance == NULL)
	{
		myInstance = new VirtualMachine();
	}

	return *myInstance;
}

void VirtualMachine::deleteInstance()
{
	if (myInstance != NULL)
	{
		delete myInstance;
		myInstance = NULL;
	}
}

VirtualMachine::VirtualMachine()
{
	
}

VirtualMachine::~VirtualMachine()
{
	
}

void VirtualMachine::run()
{
	SyslogStream &slog = SyslogStream::getInstance();

	string basePath = Settings::get("BasePath") + "Services/";
	string scriptName = "Base.js";
	string scriptFileName = basePath + "System/" + scriptName;

	if (!file_exists(scriptFileName))
	{
		slog << "Failed to load :" + scriptFileName + "\n";
	}

	string scriptSource = file_get_contents(scriptFileName);

	Handle<String> source =  String::New(scriptSource.c_str(), scriptSource.length());
	//each script name must be unique , for this demo I just run one embedded script, so the name can be fixed
	Handle<String> name = String::New(scriptName.c_str(), scriptName.length());

	// Create a template for the global object.
	myGlobal = ObjectTemplate::New();

	//associates "print" on script to the Print function
	myGlobal->Set(String::New("log"), FunctionTemplate::New(VirtualMachine_log));
	myGlobal->Set(String::New("sendCanMessage"), FunctionTemplate::New(VirtualMachine_sendCanMessage));
	myGlobal->Set(String::New("loadScript"), FunctionTemplate::New(VirtualMachine_loadScript));
	myGlobal->Set(String::New("startIntervalThread"), FunctionTemplate::New(VirtualMachine_startIntervalThread));
	myGlobal->Set(String::New("stopIntervalThread"), FunctionTemplate::New(VirtualMachine_stopIntervalThread));
	myGlobal->Set(String::New("startSocketThread"), FunctionTemplate::New(VirtualMachine_startSocketThread));
	myGlobal->Set(String::New("stopSocketThread"), FunctionTemplate::New(VirtualMachine_stopSocketThread));
	myGlobal->Set(String::New("sendToSocketThread"), FunctionTemplate::New(VirtualMachine_sendToSocketThread));

	//create context for the script
	myContext = Context::New(NULL, myGlobal);


	//access global context within this scope
	Context::Scope context_scope(myContext);
	//exception handler
	TryCatch try_catch;
	//compile script to binary code - JIT
	Handle<Script> script = Script::Compile(source, name);

	//check if we got problems on compilation
	if (script.IsEmpty())
	{

		// Print errors that happened during compilation.
		String::AsciiValue error(try_catch.Exception());
		string sError = *error;
		slog << "Failed to compile script: " + sError + "\n";
	}
	else
	{
		//no errors , let's continue
		Handle<Value> result = script->Run();

		//check if execution ended with errors
		if (result.IsEmpty())
		{
			// Print errors that happened during execution.
			String::AsciiValue error(try_catch.Exception());
			string sError = *error;
			slog << "Constructor: Failed to run script: " << sError << "\n";
		}
		else
		{
			loadScript("System/Startup.js");

			if (file_exists(basePath + "Autostart.js"))
			{
				loadScript("Autostart.js");
			}
			
			myFunctionHandleHeartbeat = Handle<Function>::Cast(myContext->Global()->Get(String::New("handleHeartbeat")));
			myFunctionOfflineCheck = Handle<Function>::Cast(myContext->Global()->Get(String::New("offlineCheck")));
			myFunctionHandleMessage = Handle<Function>::Cast(myContext->Global()->Get(String::New("handleMessage")));
			myFunctionStartup = Handle<Function>::Cast(myContext->Global()->Get(String::New("startup")));
		}
	}

	const int argc = 0;
	Handle<Value> argv[argc] = { };

	Handle<Value> result = myFunctionStartup->Call(myFunctionStartup, argc, argv); // argc and argv are your standard arguments to a function

	mySemaphore.lock();

	while (1)
	{
		string expression;

		while (myExpressions.size() > 0)
		{
			expression = myExpressions.pop();

			runExpression(expression);
		}

		CanMessage canMessage;

		while (myCanMessages.size() > 0)
		{
			canMessage = myCanMessages.pop();

			if (canMessage.isHeartbeat())
			{
				callHandleHeartbeat(stoi(canMessage.getData()["HardwareId"].getValue()));
			}
			else
			{
				callHandleMessage(canMessage);
			}
		}

		mySemaphore.wait();
	}

	mySemaphore.unlock();
}

void VirtualMachine::queueCanMessage(CanMessage canMessage)
{
	myCanMessages.push(canMessage);
	mySemaphore.broadcast();
}

void VirtualMachine::queueExpression(string expression)
{
	myExpressions.push(expression);
	mySemaphore.broadcast();
}

bool VirtualMachine::loadScript(string scriptName)
{
	SyslogStream &slog = SyslogStream::getInstance();

	string basePath = Settings::get("BasePath") + "/Services/";
	string scriptFileName = basePath + scriptName;

	if (!file_exists(scriptFileName))
	{
		slog << "Failed to load :" + scriptFileName + "\n";
		return false;
	}

	string scriptSource = file_get_contents(scriptFileName);

	Handle<String> source =  String::New(scriptSource.c_str(), scriptSource.length());
	//each script name must be unique , for this demo I just run one embedded script, so the name can be fixed
	Handle<String> name = String::New(scriptName.c_str(), scriptName.length());

	//access global context within this scope
	Context::Scope context_scope(myContext);
	//exception handler
	TryCatch try_catch;
	//compile script to binary code - JIT
	Handle<Script> script = Script::Compile(source, name);

	//check if we got problems on compilation
	if (script.IsEmpty())
	{
		// Print errors that happened during compilation.
		String::AsciiValue error(try_catch.Exception());
		string sError = *error;
		slog << "loadScript: Failed to compile script: " + sError + "\n";
		return false;
	}
	else
	{
		//no errors , let's continue
		Handle<Value> result = script->Run();

		//check if execution ended with errors
		if (result.IsEmpty())
		{
			// Print errors that happened during execution.
			String::AsciiValue error(try_catch.Exception());
			string sError = *error;
			slog << "loadScript: Failed to run script: " + sError + "\n";
			return false;
		}
	}

	return true;
}

void VirtualMachine::callHandleHeartbeat(int hardwareId)
{
	const int argc = 1;
	Handle<Value> argv[argc] = { Integer::New(hardwareId) };

	Handle<Value> result = myFunctionHandleHeartbeat->Call(myFunctionHandleHeartbeat, argc, argv); // argc and argv are your standard arguments to a function
}

void VirtualMachine::callHandleMessage(CanMessage canMessage)
{
	const int argc = 6;
	Handle<Value> argv[argc] = {String::New(canMessage.getClassName().c_str()),
								String::New(canMessage.getDirectionFlag().c_str()),
								String::New(canMessage.getModuleName().c_str()),
								Integer::New(canMessage.getModuleId()),
								String::New(canMessage.getCommandName().c_str()),
								String::New(canMessage.getJSONData().c_str()) };

	Handle<Value> result = myFunctionHandleMessage->Call(myFunctionHandleMessage, argc, argv); // argc and argv are your standard arguments to a function
}

unsigned int VirtualMachine::startIntervalThread(unsigned int timeout)
{
	IntervalThread intervalThread(timeout);

	myIntervalThreads[intervalThread.getId()] = intervalThread;
	myIntervalThreads[intervalThread.getId()].start();

	return intervalThread.getId();
}

bool VirtualMachine::stopIntervalThread(unsigned int id)
{
	if (myIntervalThreads.find(id) != myIntervalThreads.end())
	{
		myIntervalThreads.erase(id);
		return true;
	}

	return false;
}

unsigned int VirtualMachine::startSocketThread(string address, int port, unsigned int reconnectTimeout)
{
	SocketThread socketThread(address, port, reconnectTimeout);
	mySocketThreads[socketThread.getId()] = socketThread;
	mySocketThreads[socketThread.getId()].start();

	return socketThread.getId();
}

bool VirtualMachine::stopSocketThread(unsigned int id)
{
	if (mySocketThreads.find(id) != mySocketThreads.end())
	{
		mySocketThreads.erase(id);
		return true;
	}

	return false;
}

void VirtualMachine::sendToSocketThread(unsigned int id, string data)
{
	if (mySocketThreads.find(id) != mySocketThreads.end())
	{
		mySocketThreads[id].send(data);
	}
}

bool VirtualMachine::runExpression(string expression)
{
	SyslogStream &slog = SyslogStream::getInstance();

	string scriptName = "runExpression";

	Handle<String> source =  String::New(expression.c_str(), expression.length());
	//each script name must be unique , for this demo I just run one embedded script, so the name can be fixed
	Handle<String> name = String::New(scriptName.c_str(), scriptName.length());

	//access global context within this scope
	Context::Scope context_scope(myContext);
	//exception handler
	TryCatch try_catch;
	//compile script to binary code - JIT
	Handle<Script> script = Script::Compile(source, name);

	//check if we got problems on compilation
	if (script.IsEmpty())
	{
		// Print errors that happened during compilation.
		String::AsciiValue error(try_catch.Exception());
		string sError = *error;
		slog << "runExpression: Failed to compile script: " + sError + "\n";
		slog << expression << "\n\n";
		return false;
	}
	else
	{
		//no errors , let's continue
		Handle<Value> result = script->Run();

		//check if execution ended with errors
		if (result.IsEmpty())
		{
			// Print errors that happened during execution.
			String::AsciiValue error(try_catch.Exception());
			string sError = *error;
			slog << "runExpression: Failed to run script: " + sError + "\n";
			return false;
		}
	}

	return true;
}

Handle<Value> VirtualMachine_log(const Arguments& args)
{
	SyslogStream &slog = SyslogStream::getInstance();

	String::AsciiValue str(args[0]);

	slog << *str;

	return Undefined();
}

Handle<Value> VirtualMachine_sendCanMessage(const Arguments& args)
{
	//SyslogStream &slog = SyslogStream::getInstance();
	CanNetManager &canMan = CanNetManager::getInstance();

	CanMessage canMessage;

	String::AsciiValue className(args[0]);
	canMessage.setClassName(*className);
	String::AsciiValue directionFlag(args[1]);
	canMessage.setDirectionFlag(*directionFlag);
	String::AsciiValue moduleName(args[2]);
	canMessage.setModuleName(*moduleName);
	canMessage.setModuleId(args[3]->Uint32Value());
	String::AsciiValue commandName(args[4]);
	canMessage.setCommandName(*commandName);

	String::AsciiValue dataString(args[5]);

	vector<string> parts = explode(",", trim(*dataString, ','));
	map<string, CanVariable> data;

	for (int n = 0; n < parts.size(); n++)
	{
		vector<string> keyAndValue = explode(":", parts[n]);

		//string key = trim(keyAndValue[0], ' ');
		//string value = trim(keyAndValue[1], ' ');
		data[keyAndValue[0]] = CanVariable(keyAndValue[0], keyAndValue[1]);
	}

	canMessage.setData(data);
	
	canMan.sendMessage(canMessage);

	return Undefined();
}

Handle<Value> VirtualMachine_loadScript(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	String::AsciiValue str(args[0]);

	return Boolean::New(vm.loadScript(*str));
}

Handle<Value> VirtualMachine_startIntervalThread(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	unsigned int timeout = args[0]->Uint32Value();

	return Integer::New(vm.startIntervalThread(timeout));
}

Handle<Value> VirtualMachine_stopIntervalThread(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	unsigned int id = args[0]->Uint32Value();

	return Boolean::New(vm.stopIntervalThread(id));
}

Handle<Value> VirtualMachine_startSocketThread(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	String::AsciiValue address(args[0]);
	int port = args[1]->Uint32Value();
	unsigned int reconnectTimeout = args[2]->Uint32Value();

	return Integer::New(vm.startSocketThread(*address, port, reconnectTimeout));
}

Handle<Value> VirtualMachine_stopSocketThread(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	unsigned int id = args[0]->Uint32Value();

	return Boolean::New(vm.stopSocketThread(id));
}

Handle<Value> VirtualMachine_sendToSocketThread(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	unsigned int id = args[0]->Uint32Value();
	String::AsciiValue data(args[1]);

	vm.sendToSocketThread(id, *data);

	return Undefined();
}

