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

#ifndef LOGGING_LOGGER_H
#define LOGGING_LOGGER_H

#include <string>

namespace atom {
namespace logging {
    
class Logger
{
public:
    Logger(std::string name);
    virtual ~Logger();
    
    void Error(std::string message);
    void Warning(std::string message);
    void Info(std::string message);
    void Debug(std::string message);
   
private:
    std::string name_;
};    
    
}; // namespace logging
}; // namespace atom
    
#endif // LOGGING_LOGGER_H
