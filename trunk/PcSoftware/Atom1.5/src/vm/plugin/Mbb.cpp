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

#include "Mbb.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

#include "net/Manager.h"
#include "vm/Manager.h"
#include "common/common.h"
#include "common/log.h"
#include "common/exception.h"

namespace atom {
namespace vm {
namespace plugin {
    
static const std::string log_module_ = "vm::plugin::mbb";

net::ClientId Mbb::current_client_id_;
    
Mbb::Mbb(boost::asio::io_service& io_service, unsigned int port) : Plugin(io_service)
{
  this->name_ = "mbb";
  Mbb::current_client_id_ = 0;
  
  net::Manager::Instance()->ConnectSlots(net::Manager::SignalOnNewState::slot_type(&Mbb::SlotOnNewState, this, _1, _2, _3).track(this->tracker_),
                                         net::Manager::SignalOnNewData::slot_type(&Mbb::SlotOnNewData, this, _1, _2, _3).track(this->tracker_));
  
  this->ExportFunction("MbbExport_SendData", Mbb::Export_SendData);
  
  try
  {
    this->server_id_ = net::Manager::Instance()->StartServer(net::PROTOCOL_TCP, port);
    atom::log::Info(log_module_, "Started TCP server on port %u.", port);
    atom::log::Debug(log_module_, "Server id is %u", this->server_id_);
  }
  catch (std::runtime_error& e)
  {
    atom::log::Exception(log_module_, e);
  }
}

Mbb::~Mbb()
{
  net::Manager::Instance()->StopServer(this->server_id_);
  this->server_id_ = 0;
}

void Mbb::InitializeDone()
{
  Plugin::InitializeDone();
  
  this->ImportFunction("Mbb_Connected");
  this->ImportFunction("Mbb_Disconnected");
  this->ImportFunction("Mbb_ReceivedData");
}

void Mbb::CallOutput(unsigned int request_id, std::string output)
{
  atom::log::Info(log_module_, output);
}

void Mbb::SlotOnNewData(net::ClientId client_id, net::ServerId server_id, common::Byteset data)
{
  if (server_id != this->server_id_)
  {
    return;
  }
  
  this->io_service_.post(boost::bind(&Mbb::SlotOnNewDataHandler, this, client_id, server_id, data));
}

void Mbb::SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
  if (server_id != this->server_id_)
  {
    return;
  }
  
  this->io_service_.post(boost::bind(&Mbb::SlotOnNewStateHandler, this, client_id, server_id, client_state));
}

void Mbb::SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, common::Byteset data)
{
  ATOM_VM_PLUGIN_SCOPE;

  //atom::log::Debug(log_module_, "%s called!", __FUNCTION__);

  try
  {
    ArgumentListPointer call_arguments = ArgumentListPointer(new ArgumentList);
    
    if (data.GetSize() == 0)
    {
        atom::log::Error(log_module_, "Got empty data!");
        return;
    }
    
    call_arguments->push_back(v8::Integer::New(boost::lexical_cast<unsigned int>(client_id)));
    call_arguments->push_back(v8::String::New(data.ToCharString().c_str()));
        
    if (!this->Call(client_id, "Mbb_ReceivedData", call_arguments))
    {
        atom::log::Error(log_module_, "Mbb_ReceivedData failed!");
    }
    
    Mbb::current_client_id_ = 0;
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
  }
}

void Mbb::SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
  ATOM_VM_PLUGIN_SCOPE;

  //atom::log::Debug(log_module_, "%s called!", __FUNCTION__);

  try
  {
    ArgumentListPointer call_arguments = ArgumentListPointer(new ArgumentList);
    
    Mbb::current_client_id_ = client_id;
    
    call_arguments->push_back(v8::Integer::New(boost::lexical_cast<unsigned int>(client_id)));
    
    if (client_state == net::CLIENT_STATE_DISCONNECTED)
    {
      atom::log::Info(log_module_, "Client %u has disconnected.", client_id);
        
      if (!this->Call(client_id, "Mbb_Disconnected", call_arguments))
      {
          atom::log::Error(log_module_, "Mbb_Connected failed!");
      }
    }
    else if (client_state == net::CLIENT_STATE_CONNECTED)
    {
      atom::log::Info(log_module_, "Client %u has connected.", client_id);
        
      if (!this->Call(client_id, "Mbb_Connected", call_arguments))
      {
          atom::log::Error(log_module_, "Mbb_Connected failed!");
      }
    }
    else
    {
      atom::log::Error(log_module_, "Client %u reported state %u.", client_id, client_state);
      throw atom::exception::unknown_state();
    }
    
    Mbb::current_client_id_ = 0;
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
  }
}

Value Mbb::Export_SendData(const v8::Arguments& args)
{
  ATOM_VM_PLUGIN_SCOPE;

  //atom::log::Debug(log_module_, "%s called!", __FUNCTION__);
  
  try
  {
    ATOM_VM_PLUGIN_NUM_PARAMS(2);
  
    v8::String::AsciiValue data(args[1]);
    
    net::Manager::Instance()->SendTo(args[0]->Uint32Value(), std::string(*data));
    
    return handle_scope.Close(v8::Boolean::New(true));
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
