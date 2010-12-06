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

#ifndef CONTROL_MODULE_H
#define CONTROL_MODULE_H

#include <boost/shared_ptr.hpp>

#include "Node.h"

namespace atom {
namespace control {    

class Module
{
public:
    typedef boost::shared_ptr<Module> Pointer;
    typedef std::string FullId;
    typedef unsigned int Id;
    
    Module(Id id, std::string name, std::string class_name);
    virtual ~Module();
    
    static std::string MakeFullId(Id id, std::string name);
    
    FullId GetFullId();
    
    Node::Id GetNodeId();
    void SetNodeId(Node::Id node_id);
    
    Id GetId();
    std::string GetName();
    std::string GetClassName();
    
private:
    std::string full_id_;
    Node::Id node_id_;
    Id id_;
    std::string name_;
    std::string class_name_;
};

}; // namespace control
}; // namespace atom

#endif // CONTROL_MODULE_H
