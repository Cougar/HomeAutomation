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
	stop();

	///FIXME: Verify that this works and does not cause segfaults
	for (map<int, SocketThread*>::iterator iter = mySocketThreads.begin(); iter != mySocketThreads.end(); iter++)
	{
		delete iter->second;
		mySocketThreads.erase(iter);
	}
}

void VirtualMachine::run()
{
	myGlobal = ObjectTemplate::New();
	myGlobal->Set(String::New("log"), FunctionTemplate::New(VirtualMachine_log));
	myGlobal->Set(String::New("sendCanMessage"), FunctionTemplate::New(VirtualMachine_sendCanMessage));
	myGlobal->Set(String::New("sendCanNMTMessage"), FunctionTemplate::New(VirtualMachine_sendCanNMTMessage));
	myGlobal->Set(String::New("loadScript"), FunctionTemplate::New(VirtualMachine_loadScript));
	myGlobal->Set(String::New("startIntervalThread"), FunctionTemplate::New(VirtualMachine_startIntervalThread));
	myGlobal->Set(String::New("stopIntervalThread"), FunctionTemplate::New(VirtualMachine_stopIntervalThread));
	myGlobal->Set(String::New("startSocketThread"), FunctionTemplate::New(VirtualMachine_startSocketThread));
	myGlobal->Set(String::New("stopSocketThread"), FunctionTemplate::New(VirtualMachine_stopSocketThread));
	myGlobal->Set(String::New("sendToSocketThread"), FunctionTemplate::New(VirtualMachine_sendToSocketThread));
	myGlobal->Set(String::New("uint2hex"), FunctionTemplate::New(VirtualMachine_uint2hex));
	myContext = Context::New(NULL, myGlobal);


	if (loadScript("System/Base.js"))
	{
		loadScript("System/Startup.js");

		loadScript("Autostart.js");

		Context::Scope contextScope(myContext);

		myFunctionHandleNMTMessage = Handle<Function>::Cast(myContext->Global()->Get(String::New("handleNMTMessage")));
		myFunctionOfflineCheck = Handle<Function>::Cast(myContext->Global()->Get(String::New("offlineCheck")));
		myFunctionHandleMessage = Handle<Function>::Cast(myContext->Global()->Get(String::New("handleMessage")));
		myFunctionStartup = Handle<Function>::Cast(myContext->Global()->Get(String::New("startup")));
	}
	else
	{
		///FIXME: We should probably exit the application here
		return;
	}

	const int argc = 0;
	Handle<Value> argv[argc] = { };
	Handle<Value> result = myFunctionStartup->Call(myFunctionStartup, argc, argv);

	mySemaphore.lock();

	string expression;
	CanMessage canMessage;

	while (true)
	{
		while (myExpressions.size() > 0)
		{
			expression = myExpressions.pop();
			runExpression(expression);
		}

		while (myCanMessages.size() > 0)
		{
			canMessage = myCanMessages.pop();

			if (canMessage.getClassName() == "nmt")
			{
				expression = "handleNMTMessage(";
				expression += "'" + canMessage.getClassName() + "', ";
				expression += "'" + canMessage.getCommandName() + "', ";
				expression += canMessage.getJSONData();
				expression += ");";
				runExpression(expression);
			}
			else
			{
				expression = "handleMessage(";
				expression += "'" + canMessage.getClassName() + "', ";
				expression += "'" + canMessage.getDirectionFlag() + "', ";
				expression += "'" + canMessage.getModuleName() + "', ";
				expression += "" + itos(canMessage.getModuleId()) + ", ";
				expression += "'" + canMessage.getCommandName() + "', ";
				expression += canMessage.getJSONData();
				expression += ");";
				runExpression(expression);
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

	string basePath = Settings::get("BasePath") + "Services/";
	string scriptFileName = basePath + scriptName;

	if (!file_exists(scriptFileName))
	{
		slog << "Failed to load " + scriptFileName + ", file does not exist.\n";
		return false;
	}

	string scriptSource = file_get_contents(scriptFileName);

	Handle<String> source =  String::New(scriptSource.c_str(), scriptSource.length());
	Handle<String> name = String::New(scriptName.c_str(), scriptName.length());

	Context::Scope contextScope(myContext);

	TryCatch tryCatch;

	Handle<Script> script = Script::Compile(source, name);

	if (script.IsEmpty())
	{
		printException(&tryCatch);
		return false;
	}
	else
	{
		Handle<Value> result = script->Run();

		if (result.IsEmpty())
		{
			printException(&tryCatch);
			return false;
		}

		slog << "Loaded " + scriptName + "\n";
	}

	return true;
}

void VirtualMachine::callHandleNMTMessage(CanMessage canMessage)
{
	const int argc = 3;

	string jsonData = canMessage.getJSONData();

	Handle<Value> argv[argc] = {String::New(canMessage.getClassName().c_str()),
								String::New(canMessage.getCommandName().c_str()),
								String::New(jsonData.c_str()) };

	Handle<Value> result = myFunctionHandleNMTMessage->Call(myFunctionHandleNMTMessage, argc, argv); // argc and argv are your standard arguments to a function
}

void VirtualMachine::callHandleMessage(CanMessage canMessage)
{
	const int argc = 6;

	string jsonData = canMessage.getJSONData();

	Handle<Value> argv[argc] = {String::New(canMessage.getClassName().c_str()),
								String::New(canMessage.getDirectionFlag().c_str()),
								String::New(canMessage.getModuleName().c_str()),
								Integer::New(canMessage.getModuleId()),
								String::New(canMessage.getCommandName().c_str()),
								String::New(jsonData.c_str()) };

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
	SocketThread *socketThread = new SocketThread(address, port, reconnectTimeout);
	mySocketThreads[socketThread->getId()] = socketThread;
	mySocketThreads[socketThread->getId()]->start();

	return socketThread->getId();
}

bool VirtualMachine::stopSocketThread(unsigned int id)
{
	if (mySocketThreads.find(id) != mySocketThreads.end())
	{
		delete mySocketThreads[id];

		mySocketThreads.erase(id);
		return true;
	}

	return false;
}

void VirtualMachine::sendToSocketThread(unsigned int id, string data)
{
	if (mySocketThreads.find(id) != mySocketThreads.end())
	{
		mySocketThreads[id]->send(data);
	}
}

bool VirtualMachine::runExpression(string expression)
{
	string scriptName = "runExpression";

	Handle<String> source =  String::New(expression.c_str(), expression.length());
	Handle<String> name = String::New(scriptName.c_str(), scriptName.length());

	Context::Scope contextScope(myContext);

	TryCatch tryCatch;

	Handle<Script> script = Script::Compile(source, name);

	if (script.IsEmpty())
	{
		printException(&tryCatch);
		return false;
	}
	else
	{
		Handle<Value> result = script->Run();

		if (result.IsEmpty())
		{
			printException(&tryCatch);
			return false;
		}
	}

	return true;
}

void VirtualMachine::printException(TryCatch* tryCatch)
{
	SyslogStream &slog = SyslogStream::getInstance();

	HandleScope handleScope;
	String::Utf8Value exception(tryCatch->Exception());
	Handle<v8::Message> message = tryCatch->Message();

	string strException = *exception;

	if (message.IsEmpty())
	{
		slog << strException + "\n\n";
	}
	else
	{
		String::Utf8Value filename(message->GetScriptResourceName());
		string strFilename = *filename;
		
		slog << strFilename + ":" + itos(message->GetLineNumber()) + ": " + strException + "\n";

		String::Utf8Value sourceline(message->GetSourceLine());
		string strSourceline = *sourceline;

		slog << strSourceline + "\n";

		string underline = rpad("", message->GetStartColumn(), ' ');
		underline = rpad(underline, message->GetEndColumn(), '^');

		slog << underline + "\n\n";
	}
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

		data[keyAndValue[0]] = CanVariable(keyAndValue[0], keyAndValue[1]);
	}

	canMessage.setData(data);
	
	canMan.sendMessage(canMessage);

	return Undefined();
}

Handle<Value> VirtualMachine_sendCanNMTMessage(const Arguments& args)
{
	CanNetManager &canMan = CanNetManager::getInstance();

	CanMessage canMessage;

	String::AsciiValue className(args[0]);
	canMessage.setClassName(*className);
	String::AsciiValue commandName(args[1]);
	canMessage.setCommandName(*commandName);

	String::AsciiValue dataString(args[2]);

	vector<string> parts = explode(",", trim(*dataString, ','));
	map<string, CanVariable> data;

	for (int n = 0; n < parts.size(); n++)
	{
		vector<string> keyAndValue = explode(":", parts[n]);

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

Handle<Value> VirtualMachine_uint2hex(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	unsigned int id = args[0]->Uint32Value();
	unsigned int length = args[1]->Uint32Value();
	string hexId = uint2hex(id, length);

	return String::New(hexId.c_str());
}

