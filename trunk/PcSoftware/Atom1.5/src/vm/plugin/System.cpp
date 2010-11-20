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
    
System::System(bool legacy)
{
    this->name_ = "system";
    
    this->legacy_ = legacy;
                 
    this->ExportFunction("Log",        System::Export_Log);
    this->ExportFunction("LoadScript", System::Export_LoadScript);
    this->ExportFunction("Execute",    System::Export_Execute);
}

System::~System()
{

}

void System::InitializeDone()
{
    Plugin::InitializeDone();
    
    this->ImportFunction("Start");
    
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    
    arguments->push_back(v8::Boolean::New(this->legacy_));
    
    this->Call(0, "Start", arguments);
}

Value System::Export_Log(const v8::Arguments& args)
{
    if (args.Length() < 1)
    {
        LOG.Error("To few arguments to log.");
    }
    
    v8::String::AsciiValue str(args[0]);
    
    LOG.Info(*str);
    
    return v8::Undefined();
}

Value System::Export_LoadScript(const v8::Arguments& args)
{
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error("To few arguments.");
    }
    
    v8::String::AsciiValue str(args[0]);
    
    return v8::Boolean::New(Manager::Instance()->LoadScript(*str));
}

Value System::Export_Execute(const v8::Arguments& args)
{
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error("To few arguments.");
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
