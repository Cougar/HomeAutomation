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
#include "common/common.h"
#include "timer/Manager.h"
#include "storage/Manager.h"

#include "can/Message.h"

namespace atom {
namespace control {
    
Manager::Pointer Manager::instance_;

Manager::Manager() : broker::Subscriber(false), LOG("control::Manager")
{
    Node::SetupStateMachine();
    
    this->active_programming_node_id_ = 0;
    
    // Nyquist–Shannon sampling theorem state that we need to double the time, modules send every 10 seconds
    this->timer_id_ = timer::Manager::Instance()->Set(20000, true);
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

void Manager::ConnectSlots(const SignalOnNodeChange::slot_type& signal_on_node_change_, const SignalOnModuleChange::slot_type& slot_on_module_change, const SignalOnModuleMessage::slot_type& slot_on_module_message)
{
    this->signal_on_node_change_.connect(signal_on_node_change_);
    this->signal_on_module_change_.connect(slot_on_module_change);
    this->signal_on_module_message_.connect(slot_on_module_message);
}

void Manager::SlotOnTimeoutHandler(timer::TimerId timer_id, bool repeat)
{
    if (timer_id != this->timer_id_)
    {
        return;
    }
    
    for (NodeList::iterator it = this->nodes_.begin(); it != this->nodes_.end(); it++)
    {
        if (!it->second->CheckTimeout())
        {
            LOG.Info("Node " + common::ToHex(it->second->GetId()) + " has not sent anything in a long time, setting offline.");
            this->RemoveModules(it->second->GetId());
            
            storage::Manager::Instance()->FlushStore("NodeList");
            storage::Manager::Instance()->FlushStore("ModuleList");
        }
    }
}

void Manager::SlotOnMessageHandler(broker::Message::Pointer message)
{
    if (message->GetType() == broker::Message::CAN_MESSAGE)
    {
        //LOG.Debug("Message received");
        can::Message* payload = static_cast<can::Message*>(message->GetPayload().get());
        Node::Pointer node;
        
        if (payload->GetClassName() == "nmt")
        {
            if (payload->GetCommandName() == "Bios_Start")
            {
                node = this->GetNode(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"]));
                node->Trigger(Node::EVENT_BIOS_START, payload->GetVariables());
            }
            else if (payload->GetCommandName() == "App_Start")
            {
                node = this->GetNode(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"]));
                node->Trigger(Node::EVENT_APP_START, payload->GetVariables());
            }
            else if (payload->GetCommandName() == "Heartbeat")
            {
                node = this->GetNode(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"]));
                node->Trigger(Node::EVENT_HEARTBEAT, payload->GetVariables());
            }
            else if (payload->GetCommandName() == "Pgm_Ack")
            {
                node = this->GetNode(this->active_programming_node_id_);
                node->Trigger(Node::EVENT_PGM_ACK, payload->GetVariables());
            }
            else if (payload->GetCommandName() == "Pgm_Nack")
            {
                node = this->GetNode(this->active_programming_node_id_);
                node->Trigger(Node::EVENT_PGM_NACK, payload->GetVariables());
            }
        }
        else if (payload->GetDirectionName() == "From_Owner")
        {
            Module::Pointer module = this->GetModule(boost::lexical_cast<unsigned int>(payload->GetId()), payload->GetModuleName(), payload->GetClassName());
            
            if (payload->GetCommandName() == "List")
            {
                node = this->GetNode(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"]));
                node->ResetTimeout();
                
                module->SetNodeId(node->GetId());
                
                if (this->GetNumberOfModules(node->GetId()) == boost::lexical_cast<unsigned int>(payload->GetVariables()["NumberOfModules"]))
                {
                    node->Trigger(Node::EVENT_LIST_DONE, payload->GetVariables());
                }
                
                LOG.Info("Module " + module->GetFullId() + " is available on node " + common::ToHex(node->GetId()) + ".");
                this->signal_on_module_change_(module->GetFullId(), true);
            }
            else
            {
                node = this->GetNode(module->GetNodeId());
                node->ResetTimeout();
                
                this->signal_on_module_message_(module->GetFullId(), payload->GetCommandName(), payload->GetVariables());
            }
        }
    }
}

void Manager::SlotOnNewState(Node::Id node_id, Node::State current_state, Node::State target_state)
{
    if (target_state != Node::STATE_NORM_INITIALIZED)
    {
        this->RemoveModules(node_id);
    }
    
    if (target_state == Node::STATE_BPGM_OFFLINE || target_state == Node::STATE_APGM_OFFLINE)
    {
        this->active_programming_node_id_ = node_id;
    }
    else if (target_state == Node::STATE_NORM_OFFLINE)
    {
        this->active_programming_node_id_ = 0;
    }
}

void Manager::SendMessageHandler(std::string full_id, std::string command, common::StringMap variables)
{
    ModuleList::iterator it = this->modules_.find(full_id);
    
    if (it == this->modules_.end())
    {
        LOG.Warning("Can not send message to " + full_id + ", it is not available");
        return;
    }
    
    try
    {
        can::Message* payload = new can::Message(it->second->GetClassName(), "To_Owner", it->second->GetName(), it->second->GetId(), command);
        payload->SetVariables(variables);
        
        broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), this)));
        //LOG.Debug("Command " + command + " sent to " + it->second->GetName());
    }
    catch (std::runtime_error& e)
    {
        LOG.Error("Failed to send message, " + std::string(e.what()));
    }
}

void Manager::SendMessage(std::string full_id, std::string command, common::StringMap variables)
{
    this->io_service_.post(boost::bind(&Manager::SendMessageHandler, this, full_id, command, variables));
}

bool Manager::IsModuleAvailable(std::string full_id)
{
    return this->modules_.find(full_id) != this->modules_.end();
}

bool Manager::ProgramNode(Node::Id node_id, bool is_bios, std::string filename)
{
    NodeList::iterator it = this->nodes_.find(node_id);
    
    if (it == this->nodes_.end())
    {
        LOG.Error("Could not find node " +  common::ToHex(node_id));
        return false;
    }
    
    Code::Pointer code = Code::Pointer(new Code());
    if (!code->LoadIntelHexFile(filename))
    {
        LOG.Error("Failed to parse " + filename);
        return false;
    }
    
    if (is_bios)
    {
        LOG.Debug("is_bios true");
        it->second->ProgramBios(code);
    }
    else
    {
        LOG.Debug("is_bios false");
        it->second->ProgramApplication(code);
    }
    
    return true;
}

Node::Pointer Manager::GetNode(Node::Id node_id)
{
    Node::Pointer node;
    NodeList::iterator it = this->nodes_.find(node_id);
    
    if (it == this->nodes_.end())
    {
        node = Node::Pointer(new Node(node_id));
        node->ConnectSlots(Node::SignalOnNewState::slot_type(&Manager::SlotOnNewState, this, _1, _2, _3).track(this->tracker_));
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
            this->signal_on_module_change_(it->second->GetFullId(), false);
            
            this->modules_.erase(it);
        }
    }
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

}; // namespace control
}; // namespace atom
