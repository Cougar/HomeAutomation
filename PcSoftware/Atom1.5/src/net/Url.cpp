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


#include "Url.h"

#include <boost/lexical_cast.hpp>

#include "common/common.h"
#include "common/exception.h"
#include "common/log.h"

namespace atom {
namespace net {

struct Protocol
{
  std::string         name_;
  ApplicationProtocol protocol_;
  unsigned int        default_port_;
};

static const Protocol protocols_[] = {
  { "http",     APPLICATION_PROTOCOL_HTTP,    80 },
  { "ftp",      APPLICATION_PROTOCOL_FTP,     21 }
};

static const std::string log_module_ = "net::url";

  
Url::Url()
{
  this->protocol_ = APPLICATION_PROTOCOL_UNKNOWN;
  this->port_     = 0;
  this->hostname_ = "";
  this->path_     = "";
  this->raw_      = "";
}
  
Url::Url(std::string url)
{
  this->SetUrl(url);
}

std::string Url::GetRaw()
{
  return this->raw_;
}

void Url::SetUrl(std::string url)
{
  std::string::size_type position = 0;
  
  /* Set raw */
  this->raw_ = url;
  
  // http://www.google.com:80/index.html
  log::Debug(log_module_, "Parsing url: \"%s\".", url.data());
  
  
  /* Initialize member variables */
  this->protocol_ = APPLICATION_PROTOCOL_UNKNOWN;
  this->port_     = 0;
  this->hostname_ = "";
  this->path_     = "";
  
  
  /* Parse protocol */
  position = url.find(":");
  
  if (std::string::npos == position)
  {
    throw exception::parsing_failed();
  }
  
  std::string protocol_string = url.substr(0, position);
  
  log::Debug(log_module_, "Parsed protocol string: \"%s\".", protocol_string.data());
  
  for (unsigned int n = 0; n < ELEMENTS_OF(protocols_); n++)
  {
    if (protocol_string == protocols_[n].name_)
    {
      this->protocol_ = protocols_[n].protocol_;
      this->port_     = protocols_[n].default_port_;
      break;
    }
  }
  
  url.erase(0, position + 3);
  
  
  /* Parse host */
  position = url.find_first_of(":/");
  
  if (std::string::npos == position)
  {
    position = url.length();
  }
  
  this->hostname_ = url.substr(0, position);
  
  log::Debug(log_module_, "Parsed hostname string: \"%s\".", this->hostname_.data());
  
  url.erase(0, position);
  
  
  if (url.length() > 0)
  {
    /* Parse port */
    if (':' == url[0])
    {
      position = url.find_first_of("/");
  
      if (std::string::npos == position)
      {
        position = url.length();
      }
      
      this->port_ = boost::lexical_cast<unsigned int>(url.substr(1, position - 1).data());
      
      log::Debug(log_module_, "Parsed port: %u.", this->port_);
      
      url.erase(0, position);
    }
  }
   
   
  /* Parse path */
  if (url.length() > 0)
  {
    this->path_ = url;
    
    log::Debug(log_module_, "Parsed path string: \"%s\".", this->path_.data());
  }
  
}

ApplicationProtocol Url::GetProtocol()
{
  return this->protocol_;
}

unsigned int Url::GetPort()
{
  return this->port_;
}

std::string Url::GetHostname()
{
  return this->hostname_;
}

std::string Url::GetPath()
{
  return this->path_;
}


}; // namespace net
}; // namespace atom
