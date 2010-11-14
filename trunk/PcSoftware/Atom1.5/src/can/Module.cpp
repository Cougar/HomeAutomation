/*
 * 
 *    Copyright (C) 2010  Mattias Runge
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#include "Module.h"

#include <boost/lexical_cast.hpp>

namespace atom {
namespace can {

Module::Module(Module::Id id, std::string name, std::string class_name)
{
    this->id_ = id;
    this->name_ = name;
    this->class_name_ = class_name;
    this->full_id_ = Module::MakeFullId(id, name);
}

Module::~Module()
{

}

std::string Module::MakeFullId(Module::Id id, std::string name)
{
    return name + ":" + boost::lexical_cast<std::string>(id);
}

Module::FullId Module::GetFullId()
{
    return this->full_id_;
}

Module::Id Module::GetId()
{
    return this->id_;
}

Node::Id Module::GetNodeId()
{
    return this->node_id_;
}

void Module::SetNodeId(Node::Id node_id)
{
    this->node_id_ = node_id;
}

std::string Module::GetName()
{
    return this->name_;
}

std::string Module::GetClassName()
{
    return this->class_name_;
}
    
}; // namespace can
}; // namespace atom
