/*
 * 
 *  Copyright (C) 2012  Mattias Runge
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

#include "MySql.h"

#include <mysql.h>
#include <boost/lexical_cast.hpp>

#include "vm/Manager.h"
#include "common/log.h"
#include "common/exception.h"

namespace atom {
namespace vm {
namespace plugin {

MySql::Resources MySql::resources_;

static const std::string log_module_ = "vm::plugin::mysql";

MySql::MySql(boost::asio::io_service& io_service) : Plugin(io_service)
{
  this->name_ = "mysql";
  
  this->ExportFunction("MySqlExport_Connect",       MySql::Export_Connect);
  this->ExportFunction("MySqlExport_Close",         MySql::Export_Close);
  this->ExportFunction("MySqlExport_Query",         MySql::Export_Query);
  this->ExportFunction("MySqlExport_SelectDb",      MySql::Export_SelectDb);
  this->ExportFunction("MySqlExport_AffectedRows",  MySql::Export_AffectedRows);
  this->ExportFunction("MySqlExport_InsertId",      MySql::Export_InsertId);
}

MySql::~MySql()
{
}

void MySql::InitializeDone()
{
  Plugin::InitializeDone();
}

void MySql::CallOutput(unsigned int request_id, std::string output)
{
  atom::log::Info(log_module_, output);
}

MySql::ResourceId MySql::GetFreeResourceId()
{
  ResourceId resource_id = 1;
    
  while (NULL != MySql::resources_[resource_id])
  {
    resource_id++;
  }
  
  return resource_id;
}

Value MySql::Export_Connect(const v8::Arguments& args)
{
  ATOM_VM_PLUGIN_SCOPE;
  
  MYSQL*      resource = NULL;
  ResourceId  resource_id;

  //atom::log::Debug(log_module_, "%s called!", __FUNCTION__);
  
  try
  {
    ATOM_VM_PLUGIN_NUM_PARAMS(3);
  
    v8::String::AsciiValue server(args[0]);
    v8::String::AsciiValue username(args[1]);
    v8::String::AsciiValue password(args[2]);
    
    
    /* Initialize MySQL resource */
    resource = mysql_init(NULL);
  
    if (NULL == resource)
    {
      throw atom::exception::initialization_failed();
    }
    
    
    /* Connect to database */
    if (mysql_real_connect(resource, *server, *username, *password, NULL, 0, NULL, 0) == NULL)
    {
      atom::log::Error(log_module_, "Failed to connect to database %s, error code %d, error message \"%s\".", *server, mysql_errno(resource), mysql_error(resource));
      throw atom::exception::connect_failed();
    }

    
    /* Add resource to list */
    resource_id = MySql::GetFreeResourceId();
    MySql::resources_[resource_id] = resource;
  
    
    return handle_scope.Close(v8::Uint32::New(resource_id));
  }
  catch (std::exception& exception)
  {
    if (NULL != resource)
    {
      mysql_close(resource);
      resource = NULL;
    }
    
    atom::log::Exception(log_module_, exception);
    
    return handle_scope.Close(v8::Boolean::New(false));
  }
}

Value MySql::Export_Close(const v8::Arguments& args)
{
  ATOM_VM_PLUGIN_SCOPE;

  //atom::log::Debug(log_module_, "%s called!", __FUNCTION__);
  
  try
  {
    ATOM_VM_PLUGIN_NUM_PARAMS(1);
  
    
    /* Check that the resource exist */
    if (NULL == MySql::resources_[(ResourceId)args[0]->Uint32Value()])
    {
      throw atom::exception::missing_resource();
    }
    
    
    /* Close connection */
    mysql_close(MySql::resources_[(ResourceId)args[0]->Uint32Value()]);
    MySql::resources_.erase((ResourceId)args[0]->Uint32Value());
    
    
    return handle_scope.Close(v8::Boolean::New(true));
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
    
    return handle_scope.Close(v8::Boolean::New(false));
  }
}

Value MySql::Export_Query(const v8::Arguments& args)
{
  ATOM_VM_PLUGIN_SCOPE;
  
  MYSQL_RES* result = NULL;

  //atom::log::Debug(log_module_, "%s called!", __FUNCTION__);
  
  try
  {
    ArgumentListPointer result_array = ArgumentListPointer(new ArgumentList);
    
    ATOM_VM_PLUGIN_NUM_PARAMS(2);
  
    v8::String::AsciiValue query(args[1]);
    
    
    /* Check that the resource exist */
    if (NULL == MySql::resources_[(ResourceId)args[0]->Uint32Value()])
    {
      throw atom::exception::missing_resource();
    }
    
    
    /* Execute the query */
    if (0 != mysql_query(MySql::resources_[(ResourceId)args[0]->Uint32Value()], *query))
    {
      atom::log::Error(log_module_, "Failed to execute query \"%s\", error code %d, error message \"%s\".", *query, mysql_errno(MySql::resources_[(ResourceId)args[0]->Uint32Value()]), mysql_error(MySql::resources_[(ResourceId)args[0]->Uint32Value()]));
      throw atom::exception::action_failed(); 
    }
    
    
    /* Get the result */
    result = mysql_store_result(MySql::resources_[(ResourceId)args[0]->Uint32Value()]);
    
    if (NULL == result)
    {
      if (mysql_field_count(MySql::resources_[(ResourceId)args[0]->Uint32Value()]) > 0)
      {
        atom::log::Error(log_module_, "Supposed to get data but got none!");
        throw atom::exception::action_failed(); 
      }
    }
    else
    {
      
      MYSQL_ROW           row;
      uint32_t            number_of_fields = mysql_num_fields(result);
      MYSQL_FIELD*        fields = mysql_fetch_fields(result);
      uint32_t            row_index = 0;
      
      v8::Local<v8::Array> result_array = v8::Array::New();
      
      
      while (NULL != (row = mysql_fetch_row(result)))
      {
        v8::Local<v8::Object> vars = v8::Object::New();
   
        
        for (uint32_t index = 0; index < number_of_fields; index++)
        {
          vars->Set(v8::String::New(fields[index].name), v8::String::New(row[index]));
          
//          atom::log::Info(log_module_, "%s = %s\n", fields[index].name, row[index]);
        }
        
        result_array->Set(row_index, vars);
        row_index++;
      }
      
      mysql_free_result(result);
      result = NULL;
      
      return handle_scope.Close(v8::Handle<v8::Value>(result_array));
    }
    
    return handle_scope.Close(v8::Boolean::New(true));
  }
  catch (std::exception& exception)
  {
    if (NULL != result)
    {
      mysql_free_result(result);
      result = NULL;
    }
    
    atom::log::Exception(log_module_, exception);
    
    return handle_scope.Close(v8::Boolean::New(false));
  }
}

Value MySql::Export_SelectDb(const v8::Arguments& args)
{
  ATOM_VM_PLUGIN_SCOPE;

  //atom::log::Debug(log_module_, "%s called!", __FUNCTION__);
  
  try
  {
    ATOM_VM_PLUGIN_NUM_PARAMS(2);
  
    v8::String::AsciiValue database(args[1]);
    
    
    /* Check that the resource exist */
    if (NULL == MySql::resources_[(ResourceId)args[0]->Uint32Value()])
    {
      throw atom::exception::missing_resource();
    }
    
    
    /* Select database */
    if (0 != mysql_select_db(MySql::resources_[(ResourceId)args[0]->Uint32Value()], *database))
    {
      atom::log::Error(log_module_, "Failed to select database \"%s\", error code %d, error message \"%s\".", *database, mysql_errno(MySql::resources_[(ResourceId)args[0]->Uint32Value()]), mysql_error(MySql::resources_[(ResourceId)args[0]->Uint32Value()]));
      throw atom::exception::action_failed();
    }
    
    
    return handle_scope.Close(v8::Boolean::New(true));
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
    
    return handle_scope.Close(v8::Boolean::New(false));
  }
}

Value MySql::Export_AffectedRows(const v8::Arguments& args)
{
  ATOM_VM_PLUGIN_SCOPE;

  //atom::log::Debug(log_module_, "%s called!", __FUNCTION__);
  
  try
  {
    ATOM_VM_PLUGIN_NUM_PARAMS(1);
  
    
    /* Check that the resource exist */
    if (NULL == MySql::resources_[(ResourceId)args[0]->Uint32Value()])
    {
      throw atom::exception::missing_resource();
    }
    
    
    /* Select database */
    uint32_t number_of_rows = mysql_affected_rows(MySql::resources_[(ResourceId)args[0]->Uint32Value()]);
    
    
    return handle_scope.Close(v8::Uint32::New(number_of_rows));
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
    
    return handle_scope.Close(v8::Boolean::New(false));
  }
}

Value MySql::Export_InsertId(const v8::Arguments& args)
{
  ATOM_VM_PLUGIN_SCOPE;

  //atom::log::Debug(log_module_, "%s called!", __FUNCTION__);
  
  try
  {
    ATOM_VM_PLUGIN_NUM_PARAMS(1);
  
    
    /* Check that the resource exist */
    if (NULL == MySql::resources_[(ResourceId)args[0]->Uint32Value()])
    {
      throw atom::exception::missing_resource();
    }
    
    
    /* Select database */
    uint32_t insert_id = mysql_insert_id(MySql::resources_[(ResourceId)args[0]->Uint32Value()]);
    
    
    return handle_scope.Close(v8::Uint32::New(insert_id));
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
    
    return handle_scope.Close(v8::Boolean::New(false));
  }
}
  
}; // namespace plugin
}; // namespace vm
}; // namespace atom
