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

#include "Module.h"

#include <boost/lexical_cast.hpp>

#include "control/Manager.h"
#include "vm/Manager.h"

namespace atom {
namespace vm {
namespace plugin {

logging::Logger Module::LOG("vm::plugin::module");
    
Module::Module(boost::asio::io_service& io_service) : Plugin(io_service)
{
    this->name_ = "module";
    
    this->ExportFunction("Module_SendMessage", Module::Export_SendModuleMessage);
    this->ExportFunction("Module_IsModuleAvailable", Module::Export_IsModuleAvailable);
    this->ExportFunction("Module_ProgramNode", Module::Export_ProgramNode);
}

Module::~Module()
{
    this->tracker_.reset();
}

void Module::InitializeDone()
{
    Plugin::InitializeDone();
    
    this->ImportFunction("Module_OnChange");
    this->ImportFunction("Module_OnMessage");
    
    control::Manager::Instance()->ConnectSlots(control::Manager::SignalOnNodeChange::slot_type(&Module::SlotOnNodeChange, this, _1, _2).track(this->tracker_),
                                               control::Manager::SignalOnModuleChange::slot_type(&Module::SlotOnModuleChange, this, _1, _2).track(this->tracker_),
                                               control::Manager::SignalOnModuleMessage::slot_type(&Module::SlotOnModuleMessage, this, _1, _2, _3).track(this->tracker_));
}

void Module::SlotOnNodeChange(unsigned int node_id, bool available)
{
    this->io_service_.post(boost::bind(&Module::SlotOnNodeChangeHandler, this, node_id, available));
}

void Module::SlotOnModuleChange(std::string full_id, bool available)
{
    this->io_service_.post(boost::bind(&Module::SlotOnModuleChangeHandler, this, full_id, available));
}

void Module::SlotOnModuleMessage(std::string full_id, std::string command, common::StringMap variables)
{
    this->io_service_.post(boost::bind(&Module::SlotOnModuleMessageHandler, this, full_id, command, variables));
}

void Module::SlotOnNodeChangeHandler(unsigned int node_id, bool available)
{

}

void Module::SlotOnModuleChangeHandler(std::string full_id, bool available)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    arguments->push_back(v8::String::New(full_id.data()));
    arguments->push_back(v8::Boolean::New(available));
    
    this->Call(0, "Module_OnChange", arguments);
}

void Module::SlotOnModuleMessageHandler(std::string full_id, std::string command, common::StringMap variables)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    
    arguments->push_back(v8::String::New(full_id.data()));
    arguments->push_back(v8::String::New(command.data()));
    
    v8::Local<v8::Array> vars = v8::Array::New(variables.size());
    
    for (common::StringMap::iterator it = variables.begin(); it != variables.end(); it++)
    {
        vars->Set(v8::String::New(it->first.data()), v8::String::New(it->second.data()));
    }
    
    arguments->push_back(vars);
    
    this->Call(0, "Module_OnMessage", arguments);
}

Value Module::Export_SendModuleMessage(const v8::Arguments& args)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 3)
    {
        LOG.Error("To few arguments.");
    }
    
    common::StringMap variables;
    
    v8::String::AsciiValue full_id(args[0]);
    v8::String::AsciiValue command(args[1]);
    unsigned int length = args[2]->Uint32Value();
    
    for (unsigned int n = 3; n < length * 2 + 3; n += 2)
    {
        v8::String::AsciiValue name(args[n]);
        v8::String::AsciiValue value(args[n + 1]);
        
        //LOG.Debug(std::string(*name) + "====" + std::string(*value));
        
        variables[std::string(*name)] = std::string(*value);
    }
    
    control::Manager::Instance()->SendMessage(std::string(*full_id), std::string(*command), variables);
    
    return v8::Undefined();
}

Value Module::Export_IsModuleAvailable(const v8::Arguments& args)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error("To few arguments.");
    }
    
    v8::String::AsciiValue full_id(args[0]);
    
    return v8::Boolean::New(control::Manager::Instance()->IsModuleAvailable(std::string(*full_id)));
}


Value Module::Export_ProgramNode(const v8::Arguments& args)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 3)
    {
        LOG.Error("To few arguments.");
    }
    
    v8::String::AsciiValue filename(args[2]);
    
    return v8::Boolean::New(control::Manager::Instance()->ProgramNode(args[0]->Uint32Value(), args[1]->BooleanValue(), std::string(*filename)));
}
    
}; // namespace plugin
}; // namespace vm
}; // namespace atom
