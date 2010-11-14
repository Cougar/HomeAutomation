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

}

Plugin::~Plugin()
{

}

std::string Plugin::GetName()
{
    return this->name_;
}

ExportFunctionList& Plugin::GetExportFunctions()
{
    return this->export_functions_;
}

FunctionNameList& Plugin::GetImportFunctionNames()
{
    return this->import_function_names_;
}

void Plugin::InitializeDone()
{

}

void Plugin::Call(std::string name, ArgumentListPointer arguments)
{
    Manager::Instance()->Call(name, arguments);
}

void Plugin::ExportFunction(std::string name, v8::InvocationCallback function)
{
    this->export_functions_[name] = function;
}

void Plugin::ImportFunction(std::string name)
{
    this->import_function_names_.push_back(name);
}
    
}; // namespace vm
}; // namespace atom
