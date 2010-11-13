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

#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "logging/Logger.h"
#include "broker/Subscriber.h"

#include "Node.h"
#include "Module.h"

namespace atom {
namespace can {

class Manager : public broker::Subscriber
{
public:
    typedef boost::shared_ptr<Manager> Pointer;
    typedef std::map<Node::Id, Node::Pointer> NodeList;
    typedef std::map<Module::FullId, Module::Pointer> ModuleList;
    
    virtual ~Manager();
    
    static Pointer Instance();
    static void Create();
    static void Delete();
    
private:
    static Pointer instance_;
    
    NodeList nodes_;
    ModuleList modules_;
    
    Manager();
    
    void SlotOnMessageHandler(broker::Message::Pointer message);
    
    Node::Pointer GetNode(Node::Id node_id);
    Module::Pointer GetModule(Module::Id module_id, std::string module_name, std::string class_name);
    void RemoveModules(Node::Id node_id);
    void RequestModules(Node::Id node_id);
    unsigned int GetNumberOfModules(Node::Id node_id);
    
    logging::Logger LOG;
};

}; // namespace can
}; // namespace atom

#endif // CAN_MANAGER_H
