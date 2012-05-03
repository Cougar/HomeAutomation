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

#include "Logger.h"

#include <iostream>
#include <syslog.h>
#include <boost/date_time.hpp>

#include "common/log.h"

namespace atom {
namespace logging {

Logger::Logger(std::string name)
{
  this->name_ = name;
  this->name_.insert(this->name_.end(), 25 - this->name_.size(), ' ');
}

Logger::~Logger()
{
}

void Logger::Error(std::string message)
{
  log::Error(this->name_, message.c_str());
}

void Logger::Warning(std::string message)
{
  log::Warning(this->name_, message.c_str());
}

void Logger::Info(std::string message)
{
  log::Info(this->name_, message.c_str());
}

void Logger::Debug(std::string message)
{
  log::Debug(this->name_, message.c_str());
}

}; // namespace logging
}; // namespace atom
