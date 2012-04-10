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

namespace atom {
namespace vm {
namespace plugin {

logging::Logger Socket::LOG("vm::plugin::Socket");

Socket::Socket(boost::asio::io_service& io_service) : Plugin(io_service)
{
    this->name_ = "socket";
    
    this->ExportFunction("SocketExport_Connect",     Socket::Export_Connect);
    this->ExportFunction("SocketExport_Disconnect",  Socket::Export_Disconnect);
    this->ExportFunction("SocketExport_Send",        Socket::Export_Send);
}

Socket::~Socket()
{
    
}

void Socket::InitializeDone()
{
    Plugin::InitializeDone();
    
    this->ImportFunction("Socket_OnNewData");
    this->ImportFunction("Socket_OnNewState");
    
    net::Manager::Instance()->ConnectSlots(net::Manager::SignalOnNewState::slot_type(&Socket::SlotOnNewState, this, _1, _2, _3).track(this->tracker_),
                                           net::Manager::SignalOnNewData::slot_type(&Socket::SlotOnNewData, this, _1, _2, _3).track(this->tracker_));
}

void Socket::CallOutput(unsigned int request_id, std::string output)
{
    LOG.Info(output);
}

void Socket::SlotOnNewData(net::ClientId client_id, net::ServerId server_id, common::Byteset data)
{
    common::Byteset temp_buffer(data);
    this->io_service_.post(boost::bind(&Socket::SlotOnNewDataHandler, this, client_id, server_id, temp_buffer));
}

void Socket::SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    this->io_service_.post(boost::bind(&Socket::SlotOnNewStateHandler, this, client_id, server_id, client_state));
}

void Socket::SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, common::Byteset data)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    LOG.Debug(std::string(__FUNCTION__) + " called!");
     
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    arguments->push_back(v8::Integer::New(client_id));
    arguments->push_back(v8::String::New(data.ToCharString().data()));
    
    this->Call(client_id, "Socket_OnNewData", arguments);
}

void Socket::SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    arguments->push_back(v8::Integer::New(client_id));
    arguments->push_back(v8::Uint32::New((unsigned int)client_state));
    
    this->Call(client_id, "Socket_OnNewState", arguments);
}

Value Socket::Export_Connect(const v8::Arguments& args)
{
    net::ClientId client_id = 0;
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 2)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    v8::String::AsciiValue address(args[0]);
    
    try
    {
        client_id = net::Manager::Instance()->Connect(net::PROTOCOL_TCP, std::string(*address), args[1]->Uint32Value());
    }
    catch (std::runtime_error &e)
    {
    }

    return handle_scope.Close(v8::Integer::New(client_id));
}

Value Socket::Export_Disconnect(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    net::Manager::Instance()->Disconnect(args[0]->Uint32Value());
    
    return handle_scope.Close(v8::Undefined());
}

Value Socket::Export_Send(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 2)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    v8::String::AsciiValue data(args[1]);
    
    net::Manager::Instance()->SendTo(args[0]->Uint32Value(), std::string(*data));
    
    return handle_scope.Close(v8::Undefined());
}

}; // namespace plugin
}; // namespace vm
}; // namespace atom

