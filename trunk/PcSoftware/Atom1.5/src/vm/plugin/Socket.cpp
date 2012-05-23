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

#include "Socket.h"

#include <v8-debug.h>
#include <stdio.h>

#include <boost/lexical_cast.hpp>

#include "vm/Manager.h"
#include "net/Manager.h"
#include "common/common.h"
#include "common/log.h"
#include "common/exception.h"

namespace atom {
namespace vm {
namespace plugin {

static const std::string log_module_ = "vm::plugin::socket";

Socket::Socket(boost::asio::io_service& io_service) : Plugin(io_service)
{
  LOG_DEBUG_ENTER;
  
  this->name_ = "socket";
  
  this->ExportFunction("SocketExport_StartServer", Socket::Export_StartServer);
  this->ExportFunction("SocketExport_Connect",     Socket::Export_Connect);
  this->ExportFunction("SocketExport_StopServer",  Socket::Export_StopServer);
  this->ExportFunction("SocketExport_Disconnect",  Socket::Export_Disconnect);
  this->ExportFunction("SocketExport_Send",        Socket::Export_Send);
  
  LOG_DEBUG_EXIT;
}

Socket::~Socket()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
}

void Socket::InitializeDone()
{
  LOG_DEBUG_ENTER;
  
  Plugin::InitializeDone();
  
  this->ImportFunction("Socket_OnNewData");
  this->ImportFunction("Socket_OnNewClient");
  this->ImportFunction("Socket_OnNewState");
  
  net::Manager::Instance()->ConnectSlots(net::Manager::SignalOnNewState::slot_type(&Socket::SlotOnNewState, this, _1, _2).track(this->tracker_), net::Manager::SignalOnNewClient::slot_type(&Socket::SlotOnNewClient, this, _1, _2).track(this->tracker_), net::Manager::SignalOnNewData::slot_type(&Socket::SlotOnNewData, this, _1, _2).track(this->tracker_));
  
  LOG_DEBUG_EXIT;
}

void Socket::CallOutput(unsigned int request_id, std::string output)
{
  //LOG_DEBUG_ENTER;
  
  log::Info(log_module_, output);
  
  //LOG_DEBUG_EXIT;
}

void Socket::SlotOnNewData(net::SocketId id, common::Byteset data)
{
  LOG_DEBUG_ENTER;
  
  this->io_service_.post(boost::bind(&Socket::SlotOnNewDataHandler, this, id, data));
  
  LOG_DEBUG_EXIT;
}

void Socket::SlotOnNewClient(net::SocketId id, net::SocketId server_id)
{
  LOG_DEBUG_ENTER;
  
  this->io_service_.post(boost::bind(&Socket::SlotOnNewClientHandler, this, id, server_id));
  
  LOG_DEBUG_EXIT;
}

void Socket::SlotOnNewState(net::SocketId id, net::ClientState client_state)
{
  LOG_DEBUG_ENTER;
  
  this->io_service_.post(boost::bind(&Socket::SlotOnNewStateHandler, this, id, client_state));
  
  LOG_DEBUG_EXIT;
}

void Socket::SlotOnNewDataHandler(net::SocketId id, common::Byteset data)
{
  LOG_DEBUG_ENTER;
  ATOM_VM_PLUGIN_SCOPE;
  
  try
  {
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    
    if (data.size() == 0)
    {
        atom::log::Error(log_module_, "Got empty data!");
        return;
    }
    
    arguments->push_back(v8::Integer::New(boost::lexical_cast<unsigned int>(id)));
    arguments->push_back(v8::String::New(std::string(data.begin(), data.end()).data()));
        
    if (!this->Call(id, "Socket_OnNewData", arguments))
    {
      atom::log::Error(log_module_, "%s failed!", __FUNCTION__);
    }
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
  }
  
  LOG_DEBUG_EXIT;
}

void Socket::SlotOnNewClientHandler(net::SocketId id, net::SocketId server_id)
{
  LOG_DEBUG_ENTER;
  ATOM_VM_PLUGIN_SCOPE;
  
  try
  {
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
      
    arguments->push_back(v8::Integer::New(boost::lexical_cast<unsigned int>(id)));
    arguments->push_back(v8::Integer::New(boost::lexical_cast<unsigned int>(server_id)));
        
    if (!this->Call(id, "Socket_OnNewClient", arguments))
    {
      atom::log::Error(log_module_, "%s failed!", __FUNCTION__);
    }
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
  }
  
  LOG_DEBUG_EXIT;
}


void Socket::SlotOnNewStateHandler(net::SocketId id, net::ClientState client_state)
{
  LOG_DEBUG_ENTER;
  ATOM_VM_PLUGIN_SCOPE;

  try
  {
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    
    arguments->push_back(v8::Integer::New(boost::lexical_cast<unsigned int>(id)));
    arguments->push_back(v8::Uint32::New((unsigned int)client_state));
    
    if (!this->Call(id, "Socket_OnNewState", arguments))
    {
      atom::log::Error(log_module_, "%s failed!", __FUNCTION__);
    }
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
  }
  
  LOG_DEBUG_EXIT;
}

Value Socket::Export_StartServer(const v8::Arguments& args)
{
  LOG_DEBUG_ENTER
  ATOM_VM_PLUGIN_SCOPE;
  
  net::SocketId socket_id = 0;

  try
  {
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    
    if (args.Length() < 1)
    {
      throw atom::exception::missing_in_param();
    }
    
    socket_id = net::Manager::Instance()->StartServer(net::TRANSPORT_PROTOCOL_TCP, args[0]->Uint32Value());
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
    return handle_scope.Close(v8::Boolean::New(false));
  }

  LOG_DEBUG_EXIT;
  return handle_scope.Close(v8::Integer::New(socket_id));
}

Value Socket::Export_Connect(const v8::Arguments& args)
{
  LOG_DEBUG_ENTER;
  ATOM_VM_PLUGIN_SCOPE;
  
  net::SocketId socket_id = 0;

  try
  {
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    
    if (args.Length() < 2)
    {
      throw atom::exception::missing_in_param();
    }
    
    v8::String::AsciiValue address(args[0]);
    
    socket_id = net::Manager::Instance()->Connect(net::TRANSPORT_PROTOCOL_TCP, std::string(*address), args[1]->Uint32Value());
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
    return handle_scope.Close(v8::Boolean::New(false));
  }

  LOG_DEBUG_EXIT;
  return handle_scope.Close(v8::Integer::New(socket_id));
}

Value Socket::Export_StopServer(const v8::Arguments& args)
{
  LOG_DEBUG_EXIT;
  ATOM_VM_PLUGIN_SCOPE;
  
  try
  {
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    
    if (args.Length() < 1)
    {
      throw atom::exception::missing_in_param();
    }
    
    net::Manager::Instance()->StopServer(args[0]->Uint32Value());
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
    return handle_scope.Close(v8::Boolean::New(false));
  }

  LOG_DEBUG_EXIT;
  return handle_scope.Close(v8::Boolean::New(true));
}

Value Socket::Export_Disconnect(const v8::Arguments& args)
{
  LOG_DEBUG_EXIT;
  ATOM_VM_PLUGIN_SCOPE;
  
  try
  {
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    
    if (args.Length() < 1)
    {
      throw atom::exception::missing_in_param();
    }
    
    net::Manager::Instance()->Disconnect(args[0]->Uint32Value());
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
    return handle_scope.Close(v8::Boolean::New(false));
  }

  LOG_DEBUG_EXIT;
  return handle_scope.Close(v8::Boolean::New(true));
}

Value Socket::Export_Send(const v8::Arguments& args)
{
  LOG_DEBUG_EXIT;
  ATOM_VM_PLUGIN_SCOPE;
  
  try
  {
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    
    if (args.Length() < 2)
    {
      throw atom::exception::missing_in_param();
    }
    
    v8::String::AsciiValue data(args[1]);

    atom::log::Debug(log_module_, "Socket %u send %s", args[0]->Uint32Value(), *data);
    
    std::string data_string(*data);
    
    net::Manager::Instance()->SendTo(args[0]->Uint32Value(), common::Byteset(data_string.begin(), data_string.end()));
  }
  catch (std::exception& exception)
  {
    atom::log::Exception(log_module_, exception);
    return handle_scope.Close(v8::Boolean::New(false));
  }

  LOG_DEBUG_EXIT;
  return handle_scope.Close(v8::Boolean::New(true));
}

}; // namespace plugin
}; // namespace vm
}; // namespace atom
