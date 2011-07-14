/*
 * 
 *  Copyright (C) 2010  Mattias Runge
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#include "Console.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

#include "net/Manager.h"
#include "vm/Manager.h"
#include "common/common.h"

namespace atom {
namespace vm {
namespace plugin {
    
logging::Logger Console::LOG("vm::plugin::Console");
net::ClientId Console::current_client_id_;
    
Console::Console(boost::asio::io_service& io_service, unsigned int port) : Plugin(io_service)
{
    this->name_ = "console";
    Console::current_client_id_ = 0;
    
    net::Manager::Instance()->ConnectSlots(net::Manager::SignalOnNewState::slot_type(&Console::SlotOnNewState, this, _1, _2, _3).track(this->tracker_),
                                           net::Manager::SignalOnNewData::slot_type(&Console::SlotOnNewData, this, _1, _2, _3).track(this->tracker_));
    
    this->ExportFunction("ConsoleExport_PromptRequest",        Console::Export_PromptRequest);
    this->ExportFunction("ConsoleExport_AutoCompleteResponse", Console::Export_AutoCompleteResponse);
    this->ExportFunction("ConsoleExport_LogToClient",          Console::Export_LogToClient);
    this->ExportFunction("ConsoleExport_DisconnectClient",     Console::Export_DisconnectClient);
    
    try
    {
        this->server_id_ = net::Manager::Instance()->StartServer(net::PROTOCOL_TCP, port);
        LOG.Info("Started TCP server on port " + boost::lexical_cast<std::string>(port) + ".");
        LOG.Debug("Server id is " + boost::lexical_cast<std::string>(this->server_id_) + ".");
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
    }
}

Console::~Console()
{
    net::Manager::Instance()->StopServer(this->server_id_);
    this->server_id_ = 0;
}

void Console::InitializeDone()
{
    Plugin::InitializeDone();
    
    this->ImportFunction("Console_AutocompleteRequest");
    this->ImportFunction("Console_PromptResponse");
    this->ImportFunction("Console_NewConnection");
}

void Console::CallOutput(unsigned int request_id, std::string output)
{
    std::string packet = "TEXT";
    packet += common::PadNumber(output.length() + 1, 4);
    packet += output;
    
    net::Manager::Instance()->SendTo(request_id, packet);
}

void Console::SlotOnNewData(net::ClientId client_id, net::ServerId server_id, common::Byteset data)
{
    if (server_id != this->server_id_)
    {
        return;
    }
    
    this->io_service_.post(boost::bind(&Console::SlotOnNewDataHandler, this, client_id, server_id, data));
}

void Console::SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    if (server_id != this->server_id_)
    {
        return;
    }
    
    this->io_service_.post(boost::bind(&Console::SlotOnNewStateHandler, this, client_id, server_id, client_state));
}

void Console::SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, common::Byteset data)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());

    //LOG.Debug(std::string(__FUNCTION__) + " called!");

    std::string s(data.ToCharString());
    
    std::string command = s.substr(0, 4);
    unsigned int payload_length = boost::lexical_cast<unsigned int>(s.substr(4, 4));

    Console::current_client_id_ = client_id;
    
    if (command == "COMP")
    {
        ArgumentListPointer call_arguments = ArgumentListPointer(new ArgumentList);
        
        call_arguments->push_back(v8::Integer::New(boost::lexical_cast<unsigned int>(client_id)));
        call_arguments->push_back(v8::Integer::New(boost::lexical_cast<unsigned int>(s.substr(8, 4))));
        call_arguments->push_back(v8::String::New(s.substr(12).data()));
        
        if (!this->Call(client_id, "Console_AutocompleteRequest", call_arguments))
        {
            LOG.Error("Console_AutocompleteRequest failed, we are now in an undefined state!");
        }
    }
    else if (command == "RESP")
    {
        ArgumentListPointer call_arguments = ArgumentListPointer(new ArgumentList);
        
        call_arguments->push_back(v8::Integer::New(boost::lexical_cast<unsigned int>(client_id)));
        call_arguments->push_back(v8::String::New(s.substr(8).data()));
        
        if (!this->Call(client_id, "Console_PromptResponse", call_arguments))
        {
            LOG.Error("Console_PromptResponse failed, we are now in an undefined state!");
        }
    }
    else
    {
        LOG.Error("Unknown packat received, we are now in an undefined state!");
    }
    
    Console::current_client_id_ = 0;
}

void Console::SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
 
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (client_state == net::CLIENT_STATE_DISCONNECTED)
    {
        LOG.Info("Client " + boost::lexical_cast<std::string>(client_id) + " has disconnected.");
    }
    else if (client_state == net::CLIENT_STATE_CONNECTED)
    {
        LOG.Info("Client " + boost::lexical_cast<std::string>(client_id) + " has connected.");
     
        Console::current_client_id_ = client_id;
        
        ArgumentListPointer call_arguments = ArgumentListPointer(new ArgumentList);
        
        call_arguments->push_back(v8::Integer::New(boost::lexical_cast<unsigned int>(client_id)));
        
        if (!this->Call(client_id, "Console_NewConnection", call_arguments))
        {
            LOG.Error("Console_NewConnection failed, we are now in an undefined state!");
        }
        
        Console::current_client_id_ = 0;
    }
    else
    {
        LOG.Debug("Got state: " + boost::lexical_cast<std::string>((int)client_state));
    }
}

Value Console::Export_PromptRequest(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 2)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return v8::Boolean::New(false);
    }
    
    v8::String::AsciiValue prompt(args[1]);
    
    std::string packet = "PROM";
    packet += common::PadNumber(prompt.length() + 1, 4);
    packet += *prompt;
    
    net::Manager::Instance()->SendTo(args[0]->Uint32Value(), packet);
    return v8::Boolean::New(true);
}

Value Console::Export_AutoCompleteResponse(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 2)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return v8::Boolean::New(false);
    }
    
    v8::String::AsciiValue result(args[1]);
    
    std::string packet = "COMP";
    packet += common::PadNumber(result.length() + 1, 4);
    packet += *result;
    
    net::Manager::Instance()->SendTo(args[0]->Uint32Value(), packet);
    return v8::Boolean::New(true);
}

Value Console::Export_LogToClient(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 2)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return v8::Boolean::New(false);
    }
    
    v8::String::AsciiValue text(args[1]);
    
    std::string line = *text;
    
    std::string packet = "TEXT";
    packet += common::PadNumber(line.length() + 1, 4);
    packet += line;
    
    net::Manager::Instance()->SendTo(args[0]->Uint32Value(), packet);
    return v8::Boolean::New(true);
}

Value Console::Export_DisconnectClient(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return v8::Boolean::New(false);
    }
    net::Manager::Instance()->Disconnect(args[0]->Uint32Value());
    return v8::Boolean::New(true);
}

}; // namespace plugin
}; // namespace vm
}; // namespace atom
