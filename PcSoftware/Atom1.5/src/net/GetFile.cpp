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

#include "GetFile.h"
#include "Manager.h"

#include <time.h>

#include <iostream>
#include <fstream>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "common/log.h"
#include "config.h"

namespace atom {
namespace net {
  
static const std::string log_module_ = "net::getfile";

GetFile::GetFile()
{
  LOG_DEBUG_ENTER;
  

  this->filename_         = "/tmp/atomd_download_" + boost::lexical_cast<std::string>(time(NULL));
  this->header_received_  = false;
  this->status_           = false;
  this->socket_id_        = 0;
  
  LOG_DEBUG_EXIT;
}

GetFile::~GetFile()
{
  LOG_DEBUG_ENTER;
  
  net::Manager::Instance()->Disconnect(this->socket_id_);
  
  LOG_DEBUG_EXIT;
}

void GetFile::Start(Url url, const SignalOnComplete::slot_type& slot_on_complete)
{
  LOG_DEBUG_ENTER;

  this->url_              = url;
  this->filename_         = "/tmp/atomd_download_" + boost::lexical_cast<std::string>(time(NULL));
  this->header_received_  = false;
  this->status_           = false;
  this->socket_id_        = 0;
  
  this->signal_on_complete_.connect(slot_on_complete);
  
  this->socket_id_ = net::Manager::Instance()->Connect(net::TRANSPORT_PROTOCOL_TCP, this->url_.GetHostname(), this->url_.GetPort());
  
  std::string request = "GET " + this->url_.GetPath() + " HTTP/1.1\n";
  request += "Host:svn.arune.se\n";
  request += "Pragma:no-cache\n";
  request += "Referer:http://svn.arune.se/svn/HomeAutomation/trunk/Configuration/\n";
  request += "User-Agent:atomd/" + std::string(VERSION) + "\n\n";
  
  log::Debug(log_module_, request);
  
  net::Manager::Instance()->SendTo(this->socket_id_, common::Byteset(request.begin(), request.end()));
  
  LOG_DEBUG_EXIT;
}

void GetFile::SlotOnNewClientHandler(SocketId id, SocketId server_id)
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
}

void GetFile::SlotOnNewDataHandler(SocketId id, common::Byteset data)
{
  LOG_DEBUG_ENTER;
  
  if (id != this->socket_id_)
  {
    return;
  }
  
  std::string buffer(data.begin(), data.end());
  
  if (!this->header_received_)
  {
    std::string::size_type position = 0;
  
    if (!this->status_)
    {
      position = buffer.find("200 OK");
    
      if (std::string::npos == position)
      {
        log::Debug(log_module_, "No 200 found!");
        net::Manager::Instance()->Disconnect(this->socket_id_);
      }
      else
      {
        log::Debug(log_module_, "200 found!");
        this->status_ = true;
      }
    }
    
    if (this->status_)
    {
      position = buffer.find("\r\n\r\n");
  
      if (std::string::npos != position)
      {
        buffer = buffer.substr(position + 2);
        log::Debug(log_module_, "\"%s\"", buffer.data());
      }
    }
  }
  
  if (buffer.size() > 0)
  {
    std::ofstream file(this->filename_.data(), std::ios::app);
    
    file << buffer;
    
    file.close();
  }
  
  LOG_DEBUG_EXIT;
}

void GetFile::SlotOnNewStateHandler(SocketId id, ClientState client_state)
{
  LOG_DEBUG_ENTER;
  
  if (id != this->socket_id_)
  {
    return;
  }
  
  if (CLIENT_STATE_DISCONNECTED == client_state)
  {
    this->socket_id_ = 0;
    this->signal_on_complete_(this->status_, this->url_, this->filename_);
  }
  
  LOG_DEBUG_EXIT;
}


}; // namespace net
}; // namespace atom
