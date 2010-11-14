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

#include "Message.h"

#include "Protocol.h"

namespace atom {
namespace can {

Message::Message(std::string class_name, std::string direction_name, std::string module_name, unsigned int id, std::string command_name)
{
    this->class_name_ = class_name;
    this->direction_name_ = direction_name;
    this->module_name_ = module_name;
    this->id_ = id;
    this->command_name_ = command_name;
    
    xml::Node::NodeList variable_nodes;
    
    if (class_name == "nmt")
    {
        variable_nodes = Protocol::Instance()->GetNMTCommandVariables(command_name);
    }
    else
    {
        variable_nodes = Protocol::Instance()->GetCommandVariables(command_name, module_name);
    }
    
    for (unsigned int n = 0; n < variable_nodes.size(); n++)
    {
        this->variables_[variable_nodes[n].GetAttributeValue("name")] = "";
    }
}

Message::~Message()
{

}

std::string Message::GetClassName()
{
    return this->class_name_;
}

std::string Message::GetDirectionName()
{
    return this->direction_name_;
}

std::string Message::GetModuleName()
{
    return this->module_name_;
}

unsigned int Message::GetId()
{
    return this->id_;
}

std::string Message::GetCommandName()
{
    return this->command_name_;
}

std::string Message::GetVariable(std::string name)
{
    return this->variables_[name];
}

void Message::SetVariable(std::string name, std::string value)
{
    this->variables_[name] = value;
}

type::StringMap& Message::GetVariables()
{
    return this->variables_;
}

void Message::SetVariables(type::StringMap variables)
{
    this->variables_ = variables;
}

}; // namespace can
}; // namespace atom
