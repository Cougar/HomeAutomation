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

#include "Manager.h"

#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

namespace atom {
namespace vm {

Manager::Pointer Manager::instance_;

Manager::Manager() : LOG("vm::Manager")
{
}

Manager::~Manager()
{
    this->io_service_.stop();
    
    this->plugins_.clear();
    this->context_.Dispose();

    for (ImportFunctionList::iterator it = this->functions_.begin(); it != this->functions_.end(); it++)
    {
        it->second.Dispose();
    }
}

void Manager::Create()
{
    Manager::instance_ = Manager::Pointer(new Manager());
}

Manager::Pointer Manager::Instance()
{
    return Manager::instance_;
}

void Manager::Delete()
{
    Manager::instance_.reset();
}

void Manager::AddPlugin(Plugin::Pointer plugin)
{
    this->plugins_[plugin->GetName()] = plugin;
}

void Manager::Start(std::string script_path)
{
    this->script_path_ = script_path;
    this->io_service_.post(boost::bind(&Manager::StartHandler, this));
}

void Manager::Call(std::string name, ArgumentListPointer arguments)
{
    this->io_service_.post(boost::bind(&Manager::CallHandler, this, name, arguments));
}

bool Manager::LoadScript(std::string scriptname)
{
    this->io_service_.post(boost::bind(&Manager::LoadScriptHandler, this, scriptname));
}

void Manager::CallHandler(std::string name, ArgumentListPointer arguments)
{
    v8::Context::Scope context_scope(this->context_);
    v8::TryCatch try_catch;
    
    if (this->functions_.find(name) == this->functions_.end())
    {
        LOG.Error("No such function found, " + name);
        return;
    }
    
    Value result = this->functions_[name]->Call(this->context_->Global(), arguments->size(), arguments->data());
    
    if (result.IsEmpty())
    {
        LOG.Error(this->FormatException(try_catch));
    }
}

void Manager::StartHandler()
{
    v8::Handle<v8::ObjectTemplate> raw_template = v8::ObjectTemplate::New();
    
    for (PluginList::iterator it = this->plugins_.begin(); it != this->plugins_.end(); it++)
    {
        ExportFunctionList export_functions = it->second->GetExportFunctions();
        
        for (ExportFunctionList::iterator it = export_functions.begin(); it != export_functions.end(); it++)
        {
            raw_template->Set(v8::String::New(it->first.data()), v8::FunctionTemplate::New(it->second));
        }
    }
    
    this->context_ = v8::Context::New(NULL, raw_template);
    
    v8::Context::Scope context_scope(this->context_);
    
    for (PluginList::iterator it = this->plugins_.begin(); it != this->plugins_.end(); it++)
    {
        std::string scriptname = "plugin/" + it->first + ".js";
        
        if (this->LoadScriptHandler(scriptname))
        {
            FunctionNameList import_functions = it->second->GetImportFunctionNames();
            
            for (unsigned int n = 0; n < import_functions.size(); n++)
            {
                v8::Handle<v8::String> process_name = v8::String::New(import_functions[n].data());
                v8::Handle<v8::Value> process_val = this->context_->Global()->Get(process_name);
                
                if (!process_val->IsFunction())
                {
                    LOG.Error(import_functions[n] + " was not found to be a function!");
                    continue;
                }
                
                v8::Handle<v8::Function> process_fun = v8::Handle<v8::Function>::Cast(process_val);
                
                this->functions_[import_functions[n]] = v8::Persistent<v8::Function>::New(process_fun);
            }
        }
    }
    
    this->object_template_ = v8::Persistent<v8::ObjectTemplate>::New(raw_template);
    
    LOG.Info("VM initialized.");
    
    for (PluginList::iterator it = this->plugins_.begin(); it != this->plugins_.end(); it++)
    {
        it->second->InitializeDone();
    }
}

bool Manager::LoadScriptHandler(std::string scriptname)
{
    std::string path = this->script_path_ + scriptname;
    std::ifstream file(path.data());
    
    if (!file.is_open())
    {
        file.close();
        LOG.Warning("Could not load " + scriptname + ".");
        return false;
    }
    
    std::string code = "";
    std::string line;
    
    while (!file.eof())
    {
        std::getline(file, line);
        
        if (file.eof())
        {
            break;
        }
        
        code += line + "\n";
    }
    
    code += "\n";
    
    file.close();
    
    v8::Context::Scope context_scope(this->context_);
    
    v8::TryCatch try_catch;
    
    v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(code.data(), code.length()),
                                                        v8::String::New(scriptname.data(), scriptname.length()));
    
    if (script.IsEmpty())
    {
        LOG.Error(this->FormatException(try_catch));
        false;
    }
    else
    {
        Value result = script->Run();
        
        if (result.IsEmpty())
        {
            LOG.Error(this->FormatException(try_catch));
            false;
        }
    }
    
    LOG.Info("Loaded " + scriptname + " successfully!");
    return true;
}

std::string Manager::FormatException(v8::TryCatch& try_catch)
{
    std::string result;
    
    v8::HandleScope handle_scope;
    v8::String::Utf8Value exception(try_catch.Exception());
    v8::Handle<v8::Message> message = try_catch.Message();
    
    if (message.IsEmpty())
    {
        result = *exception;
    }
    else
    {
        v8::String::Utf8Value filename(message->GetScriptResourceName());
        
        result = std::string(*filename) + ":" + boost::lexical_cast<std::string>(message->GetLineNumber()) + ": " + std::string(*exception);
        
        /*v8::String::Utf8Value sourceline(message->GetSourceLine());
        string strSourceline = *sourceline;
        
        result += strSourceline + "\n";
        
        string underline = rpad("", message->GetStartColumn(), ' ');
        underline = rpad(underline, message->GetEndColumn(), '^');
        
        result += underline + "\n";*/
    }
    
    return result;
}
    
}; // namespace vm
}; // namespace atom
