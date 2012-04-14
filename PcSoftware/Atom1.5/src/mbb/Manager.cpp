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

#include "Manager.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "net/Manager.h"

namespace atom {
namespace mbb {

Manager::Pointer Manager::instance_;
  
logging::Logger Manager::LOG("mbb::Manager");
  
Manager::Manager(unsigned int port)
{
  try
  {
    this->server_id_ = net::Manager::Instance()->StartServer(net::PROTOCOL_TCP, port);
    LOG.Info("Started TCP server on port " + boost::lexical_cast<std::string>(port) + ".");
    LOG.Debug("Server id is " + boost::lexical_cast<std::string>(this->server_id_) + ".");
  }
  catch (std::runtime_error& e)
  {
    LOG.Error(e.what());
  }
}

Manager::Pointer Manager::Instance()
{
  return Manager::instance_;
}

void Manager::Create(unsigned int port)
{
  Manager::instance_ = Manager::Pointer(new Manager(port));
}

void Manager::Delete()
{
  Manager::instance_.reset();
}

Manager::~Manager()
{
  net::Manager::Instance()->StopServer(this->server_id_);
  this->server_id_ = 0;
}


void Manager::SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, common::Byteset data)
{
  if (server_id != this->server_id_)
  {
    return;
  }
  
  std::string str = data.ToCharString();
  
  std::cout << str << std::endl;
  
  //LOG.Info("Received: \"" + str + "\" from client " + boost::lexical_cast<std::string>(client_id) + " on server " + boost::lexical_cast<std::string>(server_id));
  
  if (str == "q" || str == "quit")
  {
    LOG.Info("Client " + boost::lexical_cast<std::string>(client_id) + " has requested to be disconnected.");
    net::Manager::Instance()->Disconnect(client_id);
  }
}

void Manager::SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
  if (server_id != this->server_id_)
  {
    return;
  }
  
  if (client_state == net::CLIENT_STATE_DISCONNECTED)
  {
    LOG.Info("Client " + boost::lexical_cast<std::string>(client_id) + " has disconnected.");
  }
  else if (client_state == net::CLIENT_STATE_CONNECTED)
  {
    LOG.Info("Client " + boost::lexical_cast<std::string>(client_id) + " has connected.");
  }
  else
  {
    LOG.Debug("Got state: " + boost::lexical_cast<std::string>((int)client_state));
  }
}

}; // namespace mbb
}; // namespace atom
