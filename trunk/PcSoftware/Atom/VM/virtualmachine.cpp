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

	string basePath = Settings::get("BasePath") + "/Services/System/";
	string scriptName = "Base.js";
	string scriptFileName = basePath + scriptName;

	if (!file_exists(scriptFileName))
	{
		slog << "Failed to load :" << scriptFileName << "\n";
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
	myGlobal->Set(String::New("setTimeout"), FunctionTemplate::New(VirtualMachine_setTimeout));
	myGlobal->Set(String::New("setInterval"), FunctionTemplate::New(VirtualMachine_setInterval));
	myGlobal->Set(String::New("clearTimeout"), FunctionTemplate::New(VirtualMachine_clearInterval));
	myGlobal->Set(String::New("clearInterval"), FunctionTemplate::New(VirtualMachine_clearInterval));

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
		slog << "Failed to compile script: " << *error << "\n";
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
			slog << "Constructor: Failed to run script: " << *error << "\n";
		}
		else
		{
			loadScript("System/Service.js");
			loadScript("System/ServiceManager.js");
			loadScript("System/CanMessage.js");
			loadScript("System/CanManager.js");
			loadScript("System/CanService.js");
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
		slog << "Failed to load :" << scriptFileName << "\n";
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
		slog << "Failed to compile script: " << *error << "\n";
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
			slog << "loadScript: Failed to run script: " << *error << "\n";
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

int VirtualMachine::setInterval(string expression, int interval, bool single)
{
	IntervalThread intervalThread(expression, interval, single);
	intervalThread.start();

	int timeoutId = myTimers.size();
	myTimers[timeoutId] = intervalThread;
	myTimers[timeoutId].start();

	return timeoutId;
}

bool VirtualMachine::clearInterval(int timeoutId)
{
	if (myTimers.find(timeoutId) != myTimers.end())
	{
		myTimers.erase(timeoutId);
		return true;
	}

	return false;
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
		slog << "Failed to compile script: " << *error << "\n";
		
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
			slog << "runExpression: Failed to run script: " << *error << "\n";
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
	SyslogStream &slog = SyslogStream::getInstance();
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

Handle<Value> VirtualMachine_setInterval(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	String::AsciiValue expression(args[0]);
	unsigned int interval = args[1]->Uint32Value();

	return Integer::New(vm.setInterval(*expression, interval, false));
}

Handle<Value> VirtualMachine_setTimeout(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	String::AsciiValue expression(args[0]);
	unsigned int interval = args[1]->Uint32Value();

	return Integer::New(vm.setInterval(*expression, interval, true));
}

Handle<Value> VirtualMachine_clearInterval(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	return Boolean::New(vm.clearInterval(args[0]->Uint32Value()));
}
