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

namespace atom {
namespace logging {
    
Logger::Level Logger::level_ = Logger::LEVEL_ALL;
std::ofstream Logger::file_;
boost::mutex Logger::mutex_;

Logger::Logger(std::string name)
{
    this->name_ = name;
    this->name_.insert(this->name_.end(), 20 - this->name_.size(), ' ');
    
    openlog("Atom", LOG_ODELAY, LOG_DAEMON);
}

Logger::~Logger()
{
    closelog();
}

void Logger::SetLevel(Level level)
{
    Logger::level_ = level;
}

bool Logger::OpenFile(std::string filename)
{
    Logger::file_.open(filename.data(), std::ios::app);
    
    return Logger::file_.is_open();
}

void Logger::CloseFile()
{
    Logger::file_.close();
}

void Logger::Error(std::string message)
{
    if (Logger::level_ >= Logger::LEVEL_ERROR)
    {
        this->Print(" ERROR " + this->name_ + message);
    }
}

void Logger::Warning(std::string message)
{
    if (Logger::level_ >= Logger::LEVEL_WARNING)
    {
        this->Print(" WARN  " + this->name_ + message);
    }
}

void Logger::Info(std::string message)
{
    if (Logger::level_ >= Logger::LEVEL_INFO)
    {
        this->Print(" INFO  " + this->name_ + message);
    }
}

void Logger::Debug(std::string message)
{
    if (Logger::level_ >= Logger::LEVEL_DEBUG)
    {
        this->Print(" DEBUG " + this->name_ + message);
    }
}

void Logger::Print(std::string line)
{
    this->mutex_.lock();
    
    boost::posix_time::ptime date_and_time(boost::posix_time::second_clock::local_time());
    
    std::cout << date_and_time << line << std::endl;
    
    if (Logger::file_.is_open())
    {
        Logger::file_ << date_and_time << line << std::endl;
    }
    
    syslog(LOG_INFO, "%s", line.data());
    
    this->mutex_.unlock();
}

}; // namespace logging
}; // namespace atom
