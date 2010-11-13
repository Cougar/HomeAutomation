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

#include "Manager.h"

#include <boost/lexical_cast.hpp>

#include "broker/Manager.h"
#include "broker/Message.h"
#include "Message.h"
#include <type/common.h>

namespace atom {
namespace can {
  
    
Manager::Pointer Manager::instance_;

Manager::Manager() : broker::Subscriber(false), LOG("can::Manager")
{
    // TODO: Set offline timer
}

Manager::~Manager()
{
}

Manager::Pointer Manager::Instance()
{
    return Manager::instance_;
}

void Manager::Create()
{
    Manager::instance_ = Manager::Pointer(new Manager());
}

void Manager::Delete()
{
    Manager::instance_.reset();
}

void Manager::SlotOnMessageHandler(broker::Message::Pointer message)
{
    if (message->GetType() == broker::Message::CAN_MESSAGE)
    {
        Message* payload = static_cast<Message*>(message->GetPayload().get());
        Node::Pointer node;
        
        if (payload->GetClassName() == "nmt")
        {
            if (payload->GetCommandName() == "Bios_Start")
            {
                node = this->GetNode(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"]));
                this->RemoveModules(node->GetId());
                node->SetState(Node::STATE_ONLINE);
                LOG.Info("Node " + type::ToHex(node->GetId()) + " went online.");
            }
            else if (payload->GetCommandName() == "Reset")
            {
                node = this->GetNode(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"]));
                this->RemoveModules(node->GetId());
                node->SetState(Node::STATE_OFFLINE);
                LOG.Info("Node " + type::ToHex(node->GetId()) + " went offline.");
            }
            else if (payload->GetCommandName() == "App_start")
            {
                node = this->GetNode(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"]));
                this->RequestModules(node->GetId());
                node->SetState(Node::STATE_PENDING);
                LOG.Info("Node " + type::ToHex(node->GetId()) + " started, requesting modules...");
            }
            else if (payload->GetCommandName() == "Heartbeat")
            {
                node = this->GetNode(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"]));
                
                if (node->GetState() == Node::STATE_OFFLINE || node->GetState() == Node::STATE_ONLINE)
                {
                    this->RequestModules(node->GetId());
                    node->SetState(Node::STATE_PENDING);
                    LOG.Info("Node " + type::ToHex(node->GetId()) + " was found, requesting modules...");
                }
                else if (node->GetState() == Node::STATE_INITIALIZED)
                {
                    if (this->GetNumberOfModules(node->GetId()) != boost::lexical_cast<unsigned int>(payload->GetVariables()["NumberOfModules"]))
                    {
                        this->RequestModules(node->GetId());
                        node->SetState(Node::STATE_PENDING);
                        LOG.Info("Node " + type::ToHex(node->GetId()) + " reports a different number of modules than previously known, requesting modules...");
                    }
                }
            }
        }
        else
        {
            if (payload->GetCommandName() == "List" && payload->GetDirectionName() == "From_Owner")
            {
                node = this->GetNode(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"]));
                
                Module::Pointer module = this->GetModule(boost::lexical_cast<unsigned int>(payload->GetId()), payload->GetModuleName(), payload->GetClassName());
                module->SetNodeId(node->GetId());
                
                if (this->GetNumberOfModules(node->GetId()) == boost::lexical_cast<unsigned int>(payload->GetVariables()["NumberOfModules"]))
                {
                    node->SetState(Node::STATE_INITIALIZED);
                }
                
                LOG.Info("Module " + module->GetFullId() + " is available on node " + type::ToHex(node->GetId()) + ".");
                // TODO: Notify that module is available
            }
        }
    }
}

Node::Pointer Manager::GetNode(Node::Id node_id)
{
    Node::Pointer node;
    NodeList::iterator it = this->nodes_.find(node_id);
    
    if (it == this->nodes_.end())
    {
        node = Node::Pointer(new Node(node_id));
        this->nodes_[node_id] = node;
    }
    else
    {
        node = it->second;
    }
    
    return node;
}

Module::Pointer Manager::GetModule(Module::Id module_id, std::string module_name, std::string class_name)
{
    Module::Pointer module;
    ModuleList::iterator it = this->modules_.find(Module::MakeFullId(module_id, module_name));
    
    if (it == this->modules_.end())
    {
        module = Module::Pointer(new Module(module_id, module_name, class_name));
        this->modules_[module->GetFullId()] = module;
    }
    else
    {
        module = it->second;
    }
    
    return module;
}

void Manager::RemoveModules(Node::Id node_id)
{
    for (ModuleList::iterator it = this->modules_.begin(); it != this->modules_.end(); it++)
    {
        if (it->second->GetNodeId() == node_id)
        {
            LOG.Info("Module " + it->second->GetFullId() + " is unavailable.");
            // TODO: Notify that module is unavailable
            
            this->modules_.erase(it);
        }
    }
}

void Manager::RequestModules(Node::Id node_id)
{
    this->RemoveModules(node_id);
    
    Message* payload = new Message("mnmt", "To_Owner", "", 0, "List");
    payload->SetVariable("HardwareId", boost::lexical_cast<std::string>(node_id));
    
    broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), this)));
    
    // TODO: Set timeout if no response
}

unsigned int Manager::GetNumberOfModules(Node::Id node_id)
{
    unsigned int number_of_modules = 0;
    
    for (ModuleList::iterator it = this->modules_.begin(); it != this->modules_.end(); it++)
    {
        if (it->second->GetNodeId() == node_id)
        {
            number_of_modules++;
        }
    }
    
    return number_of_modules;
}


}; // namespace can
}; // namespace atom
