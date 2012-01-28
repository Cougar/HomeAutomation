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

#include "Storage.h"

#include <boost/lexical_cast.hpp>

#include "storage/Manager.h"
#include "storage/Store.h"
#include "vm/Manager.h"

namespace atom {
namespace vm {
namespace plugin {

logging::Logger Storage::LOG("vm::plugin::storage");
    
Storage::Storage(boost::asio::io_service& io_service) : Plugin(io_service)
{
    this->name_ = "storage";
    
    this->ExportFunction("StorageExport_GetParameters", Storage::Export_GetParameters);
    this->ExportFunction("StorageExport_GetParameter",  Storage::Export_GetParameter);
    this->ExportFunction("StorageExport_SetParameter",  Storage::Export_SetParameter);
}

Storage::~Storage()
{
}

void Storage::InitializeDone()
{
    Plugin::InitializeDone();
}

void Storage::CallOutput(unsigned int request_id, std::string output)
{
    LOG.Info(output);
}

Value Storage::Export_GetParameters(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    v8::String::AsciiValue store_name(args[0]);
    
    storage::Store::ParameterList parameters = storage::Manager::Instance()->GetParameters(*store_name);
    
    v8::Local<v8::Array> params = v8::Array::New(parameters.size());

    for (storage::Store::ParameterList::iterator it = parameters.begin(); it != parameters.end(); it++)
    {
        LOG.Debug(std::string(__FUNCTION__) + ": first=\"" + it->first.data() + "\", second=\"" + it->second.data() + "\"");
        params->Set(v8::String::New(it->first.data()), v8::String::New(it->second.data()));
    }

    return handle_scope.Close(params);
}

Value Storage::Export_GetParameter(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 2)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    v8::String::AsciiValue store_name(args[0]);
    v8::String::AsciiValue parameter_name(args[1]);
    
    Value result;
    
    try
    {
        result = v8::String::New(storage::Manager::Instance()->GetParameter(*store_name, *parameter_name).data());
    }
    catch (std::runtime_error& e)
    {
        result = v8::Undefined();
    }
    
    return handle_scope.Close(result);
}

Value Storage::Export_SetParameter(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 3)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    v8::String::AsciiValue store_name(args[0]);
    v8::String::AsciiValue parameter_name(args[1]);
    v8::String::AsciiValue parameter_value(args[2]);
    
    storage::Manager::Instance()->SetParameter(*store_name, *parameter_name, *parameter_value);
    
    return handle_scope.Close(v8::String::New("OK"));
}
    
}; // namespace plugin
}; // namespace vm
}; // namespace atom
