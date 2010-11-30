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

#ifndef VM_MANAGER_H
#define VM_MANAGER_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <v8-debug.h>

#include "logging/Logger.h"
#include "common/IoService.h"

#include "net/types.h"

#include "Plugin.h"
#include "types.h"

namespace atom {
namespace vm {

class Manager : public common::IoService
{
public:
    typedef boost::shared_ptr<Manager> Pointer;
    
    virtual ~Manager();
    
    static Pointer Instance();
    static void Create();
    static void Delete();
    
    void AddPlugin(Plugin::Pointer plugin);
    void Start(std::string script_path);
    
    void Call(std::string plugin_name, unsigned int request_id, std::string name, ArgumentListPointer arguments);
    void Execute(std::string plugin_name, unsigned int request_id, std::string code);
    
    void CallHandler(std::string plugin_name, unsigned int request_id, std::string name, ArgumentListPointer arguments);
    void ExecuteHandler(std::string plugin_name, unsigned int request_id, std::string code);
    
    bool LoadScript(std::string scriptname);
    bool ImportFunction(std::string functionname);
    
    v8::Persistent<v8::Context>& GetContext();
    
private:
    typedef std::vector<Plugin::Pointer> PluginList;
    
    static Pointer instance_;
    
    std::string script_path_;
    
    v8::Persistent<v8::Context> context_;
    v8::HandleScope handle_scope_;
    v8::Persistent<v8::ObjectTemplate> object_template_;
    
    PluginList plugins_;
    ImportFunctionList functions_;
    
    Manager();
    
    void StartHandler();

    
    void SendResponse(std::string plugin_name, unsigned int request_id, std::string response);
    
    std::string FormatException(v8::TryCatch& try_catch);
    
    logging::Logger LOG;
};

}; // namespace vm
}; // namespace atom

#endif // VM_MANAGER_H
