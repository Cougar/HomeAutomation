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

v8::Persistent<v8::Context>& Manager::GetContext()
{
    return this->context_;
}

void Manager::AddPlugin(Plugin::Pointer plugin)
{
    this->plugins_.push_back(plugin);
}

void Manager::Start(std::string script_path, std::string user_script_path)
{
    this->script_path_ = script_path;
    this->user_script_path_ = user_script_path;
    this->io_service_.post(boost::bind(&Manager::StartHandler, this));
}

void Manager::CallOutput(std::string output)
{
    if (this->current_plugin_name_ != "")
    {
        for (unsigned int n = 0; n < this->plugins_.size(); n++)
        {
            if (this->plugins_[n]->GetName() == this->current_plugin_name_)
            {
                this->plugins_[n]->CallOutput(this->current_request_id_, output);
                return;
            }
        }
    }
    else
    {
        LOG.Info(output);
    }
}

Value Manager::Export_Log(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
     
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        Manager::instance_->LOG.Error(std::string(__FUNCTION__) + ": To few arguments to log.");
        return v8::Undefined();
    }
    
    v8::String::AsciiValue str(args[0]);
       
    Manager::instance_->CallOutput(*str);
    
    return handle_scope.Close(v8::Undefined());
}

void Manager::Call(std::string plugin_name, unsigned int request_id, std::string name, ArgumentListPointer arguments)
{
    this->io_service_.post(boost::bind(&Manager::CallHandler, this, plugin_name, request_id, name, arguments));
}

void Manager::Execute(std::string plugin_name, unsigned int request_id, std::string code)
{
    this->io_service_.post(boost::bind(&Manager::ExecuteHandler, this, plugin_name, request_id, code));
}

bool Manager::CallHandler(std::string plugin_name, unsigned int request_id, std::string name, ArgumentListPointer arguments)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(this->context_);
    v8::TryCatch try_catch;
    
    this->current_plugin_name_ = plugin_name;
    this->current_request_id_ = request_id;
    
    if (this->functions_.find(name) == this->functions_.end())
    {
        this->CallOutput("No such function found, " + name);
        
        this->current_plugin_name_ = "";
        this->current_request_id_ = 0;
        return false;
    }

    //LOG.Debug("Call: " + name);

    Value result = this->functions_[name]->Call(this->context_->Global(), arguments->size(), arguments->data());
    
    if (result.IsEmpty())
    {
        this->CallOutput(this->FormatException(try_catch));
        
        this->current_plugin_name_ = "";
        this->current_request_id_ = 0;
        return false;
    }
    
    //v8::String::AsciiValue response(result->ToString());
    //this->CallOutput(plugin_name, request_id, std::string(*response));
    
    this->current_plugin_name_ = "";
    this->current_request_id_ = 0;
    return true;
}

void Manager::StartHandler()
{
    v8::Locker lock;
    v8::HandleScope handle_scope;
    v8::Handle<v8::ObjectTemplate> raw_template = v8::ObjectTemplate::New();
    
    raw_template->Set(v8::String::New("Log"), v8::FunctionTemplate::New(Manager::Export_Log));
    
    for (unsigned int c = 0; c < this->plugins_.size(); c++)
    {
        ExportFunctionList export_functions = this->plugins_[c]->GetExportFunctions();
        
        for (ExportFunctionList::iterator it = export_functions.begin(); it != export_functions.end(); it++)
        {
            raw_template->Set(v8::String::New(it->first.data()), v8::FunctionTemplate::New(it->second));
        }
    }
    
    this->context_ = v8::Context::New(NULL, raw_template);
    
    
    v8::Context::Scope context_scope(this->context_);
    
    for (unsigned int c = 0; c < this->plugins_.size(); c++)
    {
        this->LoadScript("plugin/" + this->plugins_[c]->GetName() + ".js");
    }
    
    this->object_template_ = v8::Persistent<v8::ObjectTemplate>::New(raw_template);
    
    for (unsigned int c = 0; c < this->plugins_.size(); c++)
    {
        this->plugins_[c]->InitializeDone();
    }
}

bool Manager::ImportFunction(std::string functionname)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(this->context_);
    
    v8::TryCatch try_catch;
    
    v8::Handle<v8::String> process_name = v8::String::New(functionname.data());
    v8::Handle<v8::Value> process_val = this->context_->Global()->Get(process_name);
    
    if (!process_val->IsFunction())
    {
        LOG.Error(functionname + " was not found to be a function!");
        return false;
    }
    
    v8::Handle<v8::Function> process_fun = v8::Handle<v8::Function>::Cast(process_val);
    
    this->functions_[functionname] = v8::Persistent<v8::Function>::New(process_fun);
    
    return true;
}

bool Manager::LoadScript(std::string scriptname)
{
    for (unsigned int n = 0; n < this->loaded_scripts_.size(); n++)
    {
        if (this->loaded_scripts_[n] == scriptname)
        {
            LOG.Debug(scriptname + " is already loaded.");
            return true;
        }
    }
    
    std::string path = this->user_script_path_ + scriptname;
    std::ifstream file(path.data());
    
    if (!file.is_open())
    {
        path = this->script_path_ + scriptname;
        file.open(path.data());
        
        if (!file.is_open())
        {
            file.close();
            LOG.Warning("Could not load " + scriptname + ".");
            return false;
        }
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
    
    v8::Locker lock;
    v8::Context::Scope context_scope(this->context_);
    
    v8::TryCatch try_catch;
    
    v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(code.data(), code.length()),
                                                        v8::String::New(scriptname.data(), scriptname.length()));
    
    if (script.IsEmpty())
    {
        LOG.Error(this->FormatException(try_catch));
        return false;
    }
    else
    {
        Value result = script->Run();
        
        if (result.IsEmpty())
        {
            LOG.Error(this->FormatException(try_catch));
            return false;
        }
    }
    
    this->loaded_scripts_.push_back(scriptname);
    
    LOG.Info("\033[32mLoaded " + scriptname + " successfully.\033[0m");
    return true;
}

void Manager::ExecuteHandler(std::string plugin_name, unsigned int request_id, std::string code)
{
    std::string scriptname = "execute_" + plugin_name + ".js";
    
    v8::Locker lock;
    v8::Context::Scope context_scope(this->context_);
    
    v8::TryCatch try_catch;
    
    v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(code.data(), code.length()),
                                                        v8::String::New(scriptname.data(), scriptname.length()));
    
    this->current_plugin_name_ = plugin_name;
    this->current_request_id_ = request_id;
    
    if (script.IsEmpty())
    {
        this->CallOutput(this->FormatException(try_catch));
    }
    else
    {
        Value result = script->Run();
        
        if (result.IsEmpty())
        {
            this->CallOutput(this->FormatException(try_catch));
        }
        else
        {
            //v8::String::AsciiValue response(result->ToString());
            //this->CallOutput(std::string(*response));
        }
    }
    
    this->current_plugin_name_ = "";
    this->current_request_id_ = 0;
}

std::string Manager::FormatException(v8::TryCatch& try_catch)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(this->context_);
    
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
