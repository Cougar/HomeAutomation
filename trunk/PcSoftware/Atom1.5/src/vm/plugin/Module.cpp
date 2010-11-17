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

#include "can/Manager.h"

namespace atom {
namespace vm {
namespace plugin {

logging::Logger Module::LOG("vm::plugin::module");
    
Module::Module()
{
    this->name_ = "module";
    
    this->ImportFunction("Module_OnChange");
    this->ImportFunction("Module_OnMessage");
    
    this->ExportFunction("Module_SendMessage", Module::Export_SendModuleMessage);
    this->ExportFunction("Module_IsModuleAvailable", Module::Export_IsModuleAvailable);
}

Module::~Module()
{
    this->tracker_.reset();
}

void Module::InitializeDone()
{
    Plugin::InitializeDone();
    
    can::Manager::Instance()->ConnectSlots(can::Manager::SignalOnModuleChange::slot_type(&Module::SlotOnModuleChange, this, _1, _2).track(this->tracker_),
                                           can::Manager::SignalOnModuleMessage::slot_type(&Module::SlotOnModuleMessage, this, _1, _2, _3).track(this->tracker_));
}

void Module::SlotOnModuleChange(std::string full_id, bool available)
{
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    arguments->push_back(v8::String::New(full_id.data()));
    arguments->push_back(v8::Boolean::New(available));
    
    this->Call("Module_OnChange", arguments);
}

void Module::SlotOnModuleMessage(std::string full_id, std::string command, type::StringMap variables)
{
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    
    arguments->push_back(v8::String::New(full_id.data()));
    arguments->push_back(v8::String::New(command.data()));
    arguments->push_back(v8::Uint32::New(variables.size()));
    
    for (type::StringMap::iterator it = variables.begin(); it != variables.end(); it++)
    {
        arguments->push_back(v8::String::New(it->first.data()));
        arguments->push_back(v8::String::New(it->second.data()));
    }
    
    this->Call("Module_OnMessage", arguments);
}

Value Module::Export_SendModuleMessage(const v8::Arguments& args)
{
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 3)
    {
        LOG.Error("To few arguments.");
    }
    
    type::StringMap variables;
    
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
    
    can::Manager::Instance()->SendMessage(std::string(*full_id), std::string(*command), variables);
    
    return v8::Undefined();
}

Value Module::Export_IsModuleAvailable(const v8::Arguments& args)
{
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error("To few arguments.");
    }
    
    v8::String::AsciiValue full_id(args[0]);
    
    return v8::Boolean::New(can::Manager::Instance()->IsModuleAvailable(std::string(*full_id)));
}
    
}; // namespace plugin
}; // namespace vm
}; // namespace atom
