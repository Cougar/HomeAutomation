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

namespace atom {
namespace vm {
namespace plugin {
    
logging::Logger Console::LOG("vm::plugin::Console");
common::StringList Console::commands_;
    
Console::Console(boost::asio::io_service& io_service, unsigned int port) : Plugin(io_service)
{
    this->name_ = "console";
    
    net::Manager::Instance()->ConnectSlots(net::Manager::SignalOnNewState::slot_type(&Console::SlotOnNewState, this, _1, _2, _3).track(this->tracker_),
                                           net::Manager::SignalOnNewData::slot_type(&Console::SlotOnNewData, this, _1, _2, _3).track(this->tracker_));
    
    this->ExportFunction("ConsoleExport_RegisterCommand", Console::Export_RegisterCommand);
    
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
    
    this->ImportFunction("Console_DoAutocomplete");
    this->ImportFunction("Console_PromptResponse");
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
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());

    LOG.Debug(std::string(__FUNCTION__) + " called!");

    std::string str(data.ToCharString());
    
    boost::algorithm::trim_right_if(str, boost::is_any_of("\r\n"));
    
    common::StringList parts;
    
    boost::algorithm::split(parts, str, boost::is_any_of(";"), boost::algorithm::token_compress_off);
    
    if (parts[0] == "A") // Autocomplete
    {
        unsigned int arg_index = boost::lexical_cast<unsigned int>(parts[1]);
        
        std::string result;
        common::StringList arguments;
        
        boost::algorithm::split(arguments, parts[2], boost::is_any_of(" "), boost::algorithm::token_compress_on);
        
        if (arg_index == 0)
        {
            for (unsigned int n = 0; n < this->commands_.size(); n++)
            {
                //LOG.Debug(this->commands_[n]);
                result += this->commands_[n] + "\n";
            }
            
            net::Manager::Instance()->SendTo(client_id, result);
        }
        else
        {
            ArgumentListPointer call_arguments = ArgumentListPointer(new ArgumentList);
            common::StringList arguments;
            
            boost::algorithm::split(arguments, parts[2], boost::is_any_of(" "), boost::algorithm::token_compress_on);
            
            for (unsigned int n = 0; n < arguments.size() && n <= arg_index; n++)
            {
                call_arguments->push_back(v8::String::New(arguments[n].data()));
            }
            
            this->Call(client_id, "Console_DoAutocomplete", call_arguments);
        }
    }
    else if (parts[0] == "E")
    {
        ArgumentListPointer call_arguments = ArgumentListPointer(new ArgumentList);
        common::StringList arguments;
        
        boost::algorithm::split(arguments, parts[1], boost::is_any_of(" "), boost::algorithm::token_compress_on);
        
        std::string command = arguments[0];
        
        for (unsigned int n = 1; n < arguments.size(); n++)
        {
            call_arguments->push_back(v8::String::New(arguments[n].data()));
        }
        
        this->Call(client_id, command, call_arguments);
    }
    else if (parts[0] == "R")
    {
        ArgumentListPointer call_arguments = ArgumentListPointer(new ArgumentList);
        
        call_arguments->push_back(v8::String::New(parts[1].data()));
        call_arguments->push_back(v8::String::New(parts[2].data()));
        
        this->Call(client_id, "Console_PromptResponse", call_arguments);
    }
}

void Console::SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
 
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (client_state == net::CLIENT_STATE_DISCONNECTED)
    {
        LOG.Info("Client " + boost::lexical_cast<std::string>(client_id) + " has disconnected.");
    }
    else if (client_state == net::CLIENT_STATE_CONNECTED)
    {
        LOG.Info("Client " + boost::lexical_cast<std::string>(client_id) + " has connected.");
    }
    else
    {
        LOG.Debug("Got state: " + boost::lexical_cast<std::string>((int)client_state));
    }
}

void Console::ExecutionResult(std::string response, unsigned int request_id)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    LOG.Debug("ExecutionResult: response=" + response + ", request_id=" + boost::lexical_cast<std::string>(request_id));
    
    net::Manager::Instance()->SendTo(request_id, response);
}

Value Console::Export_RegisterCommand(const v8::Arguments& args)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error("To few arguments.");
    }
    
    v8::String::AsciiValue command(args[0]);
    
    if (Console::ImportFunction(std::string(*command)))
    {
        LOG.Debug(std::string(*command) + " registered successfully.");
        Console::commands_.push_back(std::string(*command));
        return v8::Boolean::New(true);
    }
    
    LOG.Debug("Failed to register command!");
    
    return v8::Boolean::New(false); 
}

}; // namespace plugin
}; // namespace vm
}; // namespace atom
