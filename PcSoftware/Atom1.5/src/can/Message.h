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

#ifndef CAN_MESSAGE_H
#define CAN_MESSAGE_H

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>

#include "common/common.h"

namespace atom {
namespace can {

class Message
{
public:
    typedef boost::shared_ptr<Message> Pointer;
    
    Message(std::string class_name, std::string direction_name, std::string module_name, unsigned int id, std::string command_name);
    virtual ~Message();
    
    std::string GetClassName();
    std::string GetDirectionName();
    std::string GetModuleName();
    unsigned int GetId();
    std::string GetCommandName();
    
    std::string GetVariable(std::string name);
    void SetVariable(std::string name, std::string value);
    
    common::StringMap& GetVariables();
    void SetVariables(common::StringMap variables);
    
private:
    std::string class_name_;
    std::string direction_name_;
    std::string module_name_;
    unsigned int id_;
    std::string command_name_;
    common::StringMap variables_;
};
        
}; // namespace can
}; // namespace atom

#endif // CAN_MESSAGE_H
