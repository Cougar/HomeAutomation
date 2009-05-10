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

	myCommandThread.stop();

	///FIXME: Verify that this works and does not cause segfaults
	for (map<unsigned long int, SocketThread*>::iterator iter = mySocketThreads.begin(); iter != mySocketThreads.end(); iter++)
	{
		iter->second->stop();
		delete iter->second;
		//mySocketThreads.erase(iter);
	}

	mySocketThreads.clear();

	for (map<int, IntervalThread*>::iterator iter = myIntervalThreads.begin(); iter != myIntervalThreads.end(); iter++)
	{
		iter->second->stop();
		delete iter->second;
		//myIntervalThreads.erase(iter);
	}

	myIntervalThreads.clear();
}

void VirtualMachine::run()
{
	myGlobal = ObjectTemplate::New();
	myGlobal->Set(String::New("_quit"), FunctionTemplate::New(VirtualMachine::_quit));
	myGlobal->Set(String::New("_print"), FunctionTemplate::New(VirtualMachine::_print));
	myGlobal->Set(String::New("log"), FunctionTemplate::New(VirtualMachine::_log));
	myGlobal->Set(String::New("sendCanMessage"), FunctionTemplate::New(VirtualMachine::_sendCanMessage));
	myGlobal->Set(String::New("sendCanNMTMessage"), FunctionTemplate::New(VirtualMachine::_sendCanNMTMessage));
	myGlobal->Set(String::New("loadScript"), FunctionTemplate::New(VirtualMachine::_loadScript));
	myGlobal->Set(String::New("startIntervalThread"), FunctionTemplate::New(VirtualMachine::_startIntervalThread));
	myGlobal->Set(String::New("stopIntervalThread"), FunctionTemplate::New(VirtualMachine::_stopIntervalThread));
	myGlobal->Set(String::New("setIntervalThreadTimeout"), FunctionTemplate::New(VirtualMachine::_setIntervalThreadTimeout));
	myGlobal->Set(String::New("startSocketThread"), FunctionTemplate::New(VirtualMachine::_startSocketThread));
	myGlobal->Set(String::New("stopSocketThread"), FunctionTemplate::New(VirtualMachine::_stopSocketThread));
	myGlobal->Set(String::New("sendToSocketThread"), FunctionTemplate::New(VirtualMachine::_sendToSocketThread));
	myGlobal->Set(String::New("loadDataStore"), FunctionTemplate::New(VirtualMachine::_loadDataStore));
	myGlobal->Set(String::New("getFileContents"), FunctionTemplate::New(VirtualMachine::_getFileContents));
	myGlobal->Set(String::New("uint2hex"), FunctionTemplate::New(VirtualMachine::_uint2hex));
	myGlobal->Set(String::New("hex2bin"), FunctionTemplate::New(VirtualMachine::_hex2bin));
	myGlobal->Set(String::New("bin2hex"), FunctionTemplate::New(VirtualMachine::_bin2hex));
	myGlobal->Set(String::New("bin2float"), FunctionTemplate::New(VirtualMachine::_bin2float));
	myGlobal->Set(String::New("float2bin"), FunctionTemplate::New(VirtualMachine::_float2bin));
	myGlobal->Set(String::New("bin2uint"), FunctionTemplate::New(VirtualMachine::_bin2uint));
	myGlobal->Set(String::New("uint2bin"), FunctionTemplate::New(VirtualMachine::_uint2bin));
	myGlobal->Set(String::New("hex2uint"), FunctionTemplate::New(VirtualMachine::_hex2uint));
	myGlobal->Set(String::New("sleep"), FunctionTemplate::New(VirtualMachine::_sleep));
	myContext = Context::New(NULL, myGlobal);


	if (loadScript("System/Base.js"))
	{
		loadScript("System/Startup.js");

		loadScript("Autostart.js");

		Context::Scope contextScope(myContext);

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


	string commandPort = Settings::get("CommandPort");
	if (commandPort != "")
	{
		myCommandThread.setSettings(stoi(commandPort), "Command console");
		myCommandThread.start();
	}
	else
	{
		Logger &log = Logger::getInstance();
		log.add("CommandPort is not defined, can not start Command console socket\n");
	}


	mySemaphore.lock();

	while (true)
	{
		while (myExpressions.size() > 0)
		{
			runExpression(myExpressions.pop());
		}

		mySemaphore.wait();
	}

	mySemaphore.unlock();
}

void VirtualMachine::queueExpression(Expression expression)
{
	myExpressions.push(expression);
	mySemaphore.broadcast();
}

string VirtualMachine::formatException(TryCatch* tryCatch)
{
	string result;

	HandleScope handleScope;
	String::Utf8Value exception(tryCatch->Exception());
	Handle<v8::Message> message = tryCatch->Message();

	string strException = *exception;

	if (message.IsEmpty())
	{
		result += strException + "\n";
	}
	else
	{
		String::Utf8Value filename(message->GetScriptResourceName());
		string strFilename = *filename;

		result += strFilename + ":" + itos(message->GetLineNumber()) + ": " + strException + "\n";

		String::Utf8Value sourceline(message->GetSourceLine());
		string strSourceline = *sourceline;

		result += strSourceline + "\n";

		string underline = rpad("", message->GetStartColumn(), ' ');
		underline = rpad(underline, message->GetEndColumn(), '^');

		result += underline + "\n";
	}

	return result;
}

bool VirtualMachine::loadDataStore(string storeName)
{
	Logger &log = Logger::getInstance();

	string basePath = Settings::get("BasePath") + "Services/DataStores/";
	string scriptFileName = basePath + storeName + ".js";

	if (!file_exists(scriptFileName))
	{
		log.add("Failed to load datastore " + scriptFileName + ", file does not exist.\n");
		return false;
	}

	string scriptSource = file_get_contents(scriptFileName);

	scriptSource = "DataStore.addStore('" + storeName + "', " + str_replace("\n", "", scriptSource) + ");";

	Handle<String> source =  String::New(scriptSource.c_str(), scriptSource.length());
	Handle<String> name = String::New(scriptFileName.c_str(), scriptFileName.length());

	Context::Scope contextScope(myContext);

	TryCatch tryCatch;

	Handle<Script> script = Script::Compile(source, name);

	if (script.IsEmpty())
	{
		log.add(formatException(&tryCatch));
		return false;
	}
	else
	{
		Handle<Value> result = script->Run();

		if (result.IsEmpty())
		{
			log.add(formatException(&tryCatch));
			return false;
		}

		log.add("Loaded datastore " + storeName + "\n");
	}

	return true;
}

bool VirtualMachine::loadScript(string scriptName)
{
	Logger &log = Logger::getInstance();

	string basePath = Settings::get("BasePath") + "Services/";
	string scriptFileName = basePath + scriptName;

	if (!file_exists(scriptFileName))
	{
		log.add("Failed to load " + scriptFileName + ", file does not exist.\n");
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
		log.addToSyslog(formatException(&tryCatch));
		return false;
	}
	else
	{
		Handle<Value> result = script->Run();

		if (result.IsEmpty())
		{
			log.addToSyslog(formatException(&tryCatch));
			return false;
		}

		log.add("Loaded " + scriptName + "\n");
	}

	return true;
}

unsigned int VirtualMachine::startIntervalThread(unsigned int timeout)
{
	IntervalThread *intervalThread = new IntervalThread(timeout);

	myIntervalThreads[intervalThread->getId()] = intervalThread;
	myIntervalThreads[intervalThread->getId()]->start();

	return intervalThread->getId();
}

bool VirtualMachine::stopIntervalThread(unsigned int id)
{
	if (myIntervalThreads.find(id) != myIntervalThreads.end())
	{
		myIntervalThreads[id]->stop();
		delete myIntervalThreads[id];
		myIntervalThreads.erase(id);

		return true;
	}

	return false;
}

bool VirtualMachine::setIntervalThreadTimeout(unsigned int id, unsigned int timeout)
{
	if (myIntervalThreads.find(id) != myIntervalThreads.end())
	{
		myIntervalThreads[id]->setTimeout(timeout);
		return true;
	}

	return false;
}

unsigned int VirtualMachine::startSocketThread(string address, int port, unsigned int reconnectTimeout)
{
	SocketThread *socketThread = new SocketThread(address, port, reconnectTimeout);
	mySocketThreads[socketThread->getId()] = socketThread;
	mySocketThreads[socketThread->getId()]->start();
	mySocketThreads[socketThread->getId()]->startSocket();

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

void VirtualMachine::disconnectCommandClient(unsigned int id)
{
	myCommandThread.disconnectClient(id);
}

void VirtualMachine::sendToCommandClient(unsigned int id, string data)
{
	myCommandThread.sendTo(id, data);
}

void VirtualMachine::print(unsigned int id, string data)
{
	if (id == 0)
	{
		Logger &log = Logger::getInstance();
		log.add(data);
	}
	else
	{
		VirtualMachine &vm = VirtualMachine::getInstance();
		vm.sendToCommandClient(id, data);
	}
}

bool VirtualMachine::runExpression(Expression expression)
{
	string scriptName = "runExpression";

	string scriptData = expression.getScript();

	scriptData = "setClientId(" + itos(expression.getTargetId()) + ");\n" + scriptData + "\n";

	Handle<String> source =  String::New(scriptData.c_str(), scriptData.length());
	Handle<String> name = String::New(scriptName.c_str(), scriptName.length());

	Context::Scope contextScope(myContext);

	TryCatch tryCatch;

	Handle<Script> script = Script::Compile(source, name);

	if (script.IsEmpty())
	{
		print(expression.getTargetId(), formatException(&tryCatch));
		return false;
	}
	else
	{
		Handle<Value> result = script->Run();

		if (result.IsEmpty())
		{
			print(expression.getTargetId(), formatException(&tryCatch));
			return false;
		}
		else if (expression.getTargetId() != 0)
		{
			 String::AsciiValue ascii(result);
			 string resultString = *ascii;

			 if (resultString != "undefined")
			 {
				print(expression.getTargetId(), resultString + "\n");
			 }
		}
	}

	return true;
}


Handle<Value> VirtualMachine::_quit(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	vm.disconnectCommandClient(args[0]->Uint32Value());

	return Undefined();
}

Handle<Value> VirtualMachine::_log(const Arguments& args)
{
	Logger &log = Logger::getInstance();

	String::AsciiValue str(args[0]);

	log.add(*str);

	return Undefined();
}

Handle<Value> VirtualMachine::_print(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	String::AsciiValue str(args[1]);

	vm.print(args[0]->Uint32Value(), *str);

	return Undefined();
}

Handle<Value> VirtualMachine::_sendCanMessage(const Arguments& args)
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

Handle<Value> VirtualMachine::_sendCanNMTMessage(const Arguments& args)
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

Handle<Value> VirtualMachine::_loadScript(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	String::AsciiValue str(args[0]);

	return Boolean::New(vm.loadScript(*str));
}

Handle<Value> VirtualMachine::_loadDataStore(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	String::AsciiValue str(args[0]);

	return Boolean::New(vm.loadDataStore(*str));
}

Handle<Value> VirtualMachine::_startIntervalThread(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();
	return Integer::New(vm.startIntervalThread(args[0]->Uint32Value()));
}

Handle<Value> VirtualMachine::_stopIntervalThread(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();
	return Boolean::New(vm.stopIntervalThread(args[0]->Uint32Value()));
}

Handle<Value> VirtualMachine::_setIntervalThreadTimeout(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();
	return Integer::New(vm.setIntervalThreadTimeout(args[0]->Uint32Value(), args[1]->Uint32Value()));
}

Handle<Value> VirtualMachine::_startSocketThread(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	String::AsciiValue address(args[0]);

	return Integer::New(vm.startSocketThread(*address, args[1]->Uint32Value(), args[2]->Uint32Value()));
}

Handle<Value> VirtualMachine::_stopSocketThread(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	return Boolean::New(vm.stopSocketThread(args[0]->Uint32Value()));
}

Handle<Value> VirtualMachine::_sendToSocketThread(const Arguments& args)
{
	VirtualMachine &vm = VirtualMachine::getInstance();

	String::AsciiValue data(args[1]);

	vm.sendToSocketThread(args[0]->Uint32Value(), *data);

	return Undefined();
}

Handle<Value> VirtualMachine::_getFileContents(const Arguments& args)
{
	String::AsciiValue filename(args[0]);

	if (!file_exists(*filename))
	{
		return Undefined();
	}

	string content = file_get_contents(*filename);

	return String::New(content.c_str());
}

Handle<Value> VirtualMachine::_uint2hex(const Arguments& args)
{
	string hex = uint2hex(args[0]->Uint32Value(), args[1]->Uint32Value());
	return String::New(hex.c_str());
}

Handle<Value> VirtualMachine::_hex2bin(const Arguments& args)
{
	String::AsciiValue hex(args[0]);
	string bin = hex2bin(*hex);
	return String::New(bin.c_str());
}

Handle<Value> VirtualMachine::_bin2hex(const Arguments& args)
{
	String::AsciiValue bin(args[0]);
	string hex = bin2hex(*bin);
	return String::New(hex.c_str());
}

Handle<Value> VirtualMachine::_bin2float(const Arguments& args)
{
	String::AsciiValue bin(args[0]);
	float f = bin2float(*bin);
	return Number::New(f);
}

Handle<Value> VirtualMachine::_float2bin(const Arguments& args)
{
	string bin = float2bin(args[0]->NumberValue(), args[1]->Uint32Value());
	return String::New(bin.c_str());
}

Handle<Value> VirtualMachine::_bin2uint(const Arguments& args)
{
	String::AsciiValue bin(args[0]);
	unsigned int num = bin2uint(*bin);
	return Number::New(num);
}

Handle<Value> VirtualMachine::_uint2bin(const Arguments& args)
{
	string bin = uint2bin(args[0]->Uint32Value(), args[1]->Uint32Value());
	return String::New(bin.c_str());
}

Handle<Value> VirtualMachine::_hex2uint(const Arguments& args)
{
	String::AsciiValue hex(args[0]);
	unsigned int num = hex2uint(*hex);
	return Number::New(num);
}

Handle<Value> VirtualMachine::_sleep(const Arguments& args)
{
	usleep(args[0]->Uint32Value());
	return Undefined();
}
