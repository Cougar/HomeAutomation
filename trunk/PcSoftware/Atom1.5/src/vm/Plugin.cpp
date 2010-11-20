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

#include "Plugin.h"

#include "Manager.h"

namespace atom {
namespace vm {
    
Plugin::Plugin()
{
    this->tracker_ = TrackerPointer(new char);
}

Plugin::~Plugin()
{
    this->tracker_.reset();
}

std::string Plugin::GetName()
{
    return this->name_;
}

ExportFunctionList& Plugin::GetExportFunctions()
{
    return this->export_functions_;
}

void Plugin::InitializeDone()
{

}

void Plugin::ExecutionResult(std::string response, unsigned int request_id)
{
    
}

void Plugin::Call(unsigned int request_id, std::string name, ArgumentListPointer arguments)
{
    Manager::Instance()->Call(this->name_, request_id, name, arguments);
}

void Plugin::Execute(unsigned int request_id, std::string code)
{
    Manager::Instance()->Execute(this->name_, request_id, code);
}

void Plugin::ExportFunction(std::string name, v8::InvocationCallback function)
{
    this->export_functions_[name] = function;
}

bool Plugin::ImportFunction(std::string name)
{
    return Manager::Instance()->ImportFunction(name);
}
    
}; // namespace vm
}; // namespace atom
