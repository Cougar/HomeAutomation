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

#include "System.h"

#include <v8-debug.h>
#include <stdio.h>

#include "vm/Manager.h"

namespace atom {
namespace vm {
namespace plugin {

logging::Logger System::LOG("vm::plugin::System");
    
System::System(boost::asio::io_service& io_service) : Plugin(io_service)
{
    this->name_ = "system";
    
    this->ExportFunction("Require",    System::Export_Require);
    this->ExportFunction("Execute",    System::Export_Execute);
}

System::~System()
{

}

void System::InitializeDone()
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    Plugin::InitializeDone();
    
    v8::Boolean::New(Manager::Instance()->LoadScript("interface/list.js"));
    v8::Boolean::New(Manager::Instance()->LoadScript("autostart.js"));
}

void System::CallOutput(unsigned int request_id, std::string output)
{
    LOG.Info(output);
}

Value System::Export_Require(const v8::Arguments& args)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return v8::Boolean::New(false);
    }
    
    v8::String::AsciiValue str(args[0]);
    
    return v8::Boolean::New(Manager::Instance()->LoadScript(*str));
}

Value System::Export_Execute(const v8::Arguments& args)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return v8::Boolean::New(false);
    }
    
    v8::String::AsciiValue command(args[0]);
    
    std::string output_buffer;
    char output[128];
    
    FILE* pipe = popen(*command, "r" );
    
    if (pipe == NULL)
    {
        LOG.Info("Failed to execute \"" + std::string(*command) + "\".");
        return v8::Boolean::New(false);
    }
    
    while (!feof(pipe))
    {
        if (fgets(output, 128, pipe) != NULL)
        {
            output_buffer += output;
        }
    }
    
    pclose(pipe);
    
    return v8::String::New(output_buffer.data());
}
    
}; // namespace plugin
}; // namespace vm
}; // namespace atom
