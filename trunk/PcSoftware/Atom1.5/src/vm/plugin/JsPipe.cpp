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

#include "JsPipe.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

#include "net/Manager.h"
#include "vm/Manager.h"
#include "common/common.h"
#include "common/log.h"

namespace atom {
namespace vm {
namespace plugin {

static const std::string log_module_ = "vm::plugin::jspipe";

JsPipe::JsPipe(boost::asio::io_service& io_service, unsigned int port) : Plugin(io_service)
{
  this->name_ = "jspipe";

  net::Manager::Instance()->ConnectSlots(net::Manager::SignalOnNewState::slot_type(&JsPipe::SlotOnNewState, this, _1, _2).track(this->tracker_),
                                         net::Manager::SignalOnNewClient::slot_type(&JsPipe::SlotOnNewClient, this, _1, _2).track(this->tracker_),
                                         net::Manager::SignalOnNewData::slot_type(&JsPipe::SlotOnNewData, this, _1, _2).track(this->tracker_));

  this->ExportFunction("JsPipeExport_DisconnectClient", JsPipe::Export_DisconnectClient);

  try
  {
    this->server_id_ = net::Manager::Instance()->StartServer(net::TRANSPORT_PROTOCOL_TCP, port);
    log::Info(log_module_, "Started JsPipe TCP server on port %u.", port);
    log::Debug(log_module_, "Server id is %u.", this->server_id_);
  }
  catch (std::runtime_error& e)
  {
    log::Exception(log_module_, e);
  }
}

JsPipe::~JsPipe()
{
  net::Manager::Instance()->StopServer(this->server_id_);
  this->server_id_ = 0;
}

void JsPipe::InitializeDone()
{
  Plugin::InitializeDone();
}

void JsPipe::CallResult(unsigned int request_id, std::string output)
{
  output += "\n";

  net::Manager::Instance()->SendTo(request_id, common::Byteset(output.begin(), output.end()));
}

void JsPipe::SlotOnNewData(net::SocketId id, common::Byteset data)
{
  this->io_service_.post(boost::bind(&JsPipe::SlotOnNewDataHandler, this, id, data));
}

void JsPipe::SlotOnNewClient(net::SocketId id, net::SocketId server_id)
{
  this->io_service_.post(boost::bind(&JsPipe::SlotOnNewClientHandler, this, id, server_id));
}

void JsPipe::SlotOnNewState(net::SocketId id, net::ClientState client_state)
{
  this->io_service_.post(boost::bind(&JsPipe::SlotOnNewStateHandler, this, id, client_state));
}

void JsPipe::SlotOnNewDataHandler(net::SocketId id, common::Byteset data)
{
  v8::Locker lock;
  v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
  v8::HandleScope handle_scope;

  if (std::find(this->clients_.begin(), this->clients_.end(), id) == this->clients_.end())
  {
    return;
  }


  //log::Debug(log_module_, "%s called!", __FUNCTION__);

  if (data.size() == 0)
  {
    log::Error("%s got empty data!", __FUNCTION__);
    return;
  }

  std::string s(data.begin(), data.end());

  this->Execute(id, s);
}

void JsPipe::SlotOnNewClientHandler(net::SocketId id, net::SocketId server_id)
{
  if (server_id == this->server_id_)
  {
    this->clients_.insert(id);
  }
}

void JsPipe::SlotOnNewStateHandler(net::SocketId id, net::ClientState client_state)
{
  v8::Locker lock;
  v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
  v8::HandleScope handle_scope;

  if (std::find(this->clients_.begin(), this->clients_.end(), id) == this->clients_.end())
  {
    return;
  }

  //log::Debug(log_module_, "%s called!", __FUNCTION__);

  if (client_state == net::CLIENT_STATE_DISCONNECTED)
  {
    log::Info(log_module_, "Client %u has disconnected from JsPipe.", id);
    this->clients_.erase(id);
  }
  else if (client_state == net::CLIENT_STATE_CONNECTED)
  {
    log::Info(log_module_, "Client %u has connected to JsPipe.", id);
  }
  else
  {
    log::Debug(log_module_, "Got state: %u", client_state);
  }
}

Value JsPipe::Export_DisconnectClient(const v8::Arguments& args)
{
  v8::Locker lock;
  v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
  v8::HandleScope handle_scope;

  //log::Debug(log_module_, "%s called!", __FUNCTION__);

  if (args.Length() < 1)
  {
    log::Error(log_module_, "%s: To few arguments.", __FUNCTION__);
    return handle_scope.Close(v8::Boolean::New(false));
  }

  net::Manager::Instance()->Disconnect(args[0]->Uint32Value());
  return handle_scope.Close(v8::Boolean::New(true));
}

}; // namespace plugin
}; // namespace vm
}; // namespace atom
