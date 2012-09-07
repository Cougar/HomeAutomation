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

#include "Forward.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "net/Manager.h"
#include "broker/Manager.h"
#include "common/Byteset.h"

#include "Message.h"

namespace atom {
namespace can {

Forward::Forward(unsigned int port): broker::Subscriber(port), LOG("can::Forward")
{
    try
    {
        this->server_id_ = net::Manager::Instance()->StartServer(net::TRANSPORT_PROTOCOL_TCP, port);
        LOG.Info("Started TCP CAN forward server on port " + boost::lexical_cast<std::string>(port) + ".");
        LOG.Debug("Server id is " + boost::lexical_cast<std::string>(this->server_id_) + ".");
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
    }
}

Forward::~Forward()
{
    net::Manager::Instance()->StopServer(this->server_id_);
    this->server_id_ = 0;
}


void Forward::SlotOnMessageHandler(broker::Message::Pointer message)
{
    if (message->GetType() == broker::Message::CAN_RAW_BYTES)
    {
      net::Manager::Instance()->SendToAll(this->server_id_, message->GetRawData());
    }
}

void Forward::SlotOnNewDataHandler(net::SocketId id, common::Byteset data)
{
    if (std::find(this->clients_.begin(), this->clients_.end(), id) == this->clients_.end())
    {
      return;
    }

  broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_RAW_BYTES, data, this)));
}

void Forward::SlotOnNewClientHandler(net::SocketId id, net::SocketId server_id)
{
  if (server_id == this->server_id_)
  {
    this->clients_.insert(id);
  }
}

void Forward::SlotOnNewStateHandler(net::SocketId id, net::ClientState client_state)
{
    if (std::find(this->clients_.begin(), this->clients_.end(), id) == this->clients_.end())
    {
      return;
    }

    if (client_state == net::CLIENT_STATE_DISCONNECTED)
    {
        LOG.Debug("Client " + boost::lexical_cast<std::string>(id) + " has disconnected.");
        this->clients_.erase(id);
    }
    else if (client_state == net::CLIENT_STATE_CONNECTED)
    {
        LOG.Debug("Client " + boost::lexical_cast<std::string>(id) + " has connected.");
    }
    else
    {
        LOG.Debug("Got state: " + boost::lexical_cast<std::string>((int)client_state));
    }
}

}; // namespace can
}; // namespace atom
