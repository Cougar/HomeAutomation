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
#include "common/log.h"

#include "can/Message.h"

namespace atom {
namespace control {
    
Manager::Pointer Manager::instance_;

static const std::string log_module_ = "control::manager";

Manager::Manager() : broker::Subscriber(false)
{
  LOG_DEBUG_ENTER;
  
  Node::SetupStateMachine();
  
  this->active_programming_node_id_ = "";
  
  // Nyquist–Shannon sampling theorem state that we need to double the time, modules send every 10 seconds
  this->timer_id_ = timer::Manager::Instance()->SetTimer(20000, true);
  
  LOG_DEBUG_EXIT;
}

Manager::~Manager()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
}

Manager::Pointer Manager::Instance()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
  
  return Manager::instance_;
}

void Manager::Create()
{
  LOG_DEBUG_ENTER;
  
  Manager::instance_ = Manager::Pointer(new Manager());
  
  LOG_DEBUG_EXIT;
}

void Manager::Delete()
{
  LOG_DEBUG_ENTER;
  
  Manager::instance_.reset();
  
  LOG_DEBUG_EXIT;
}

void Manager::ConnectSlotNode(const SignalOnNodeChange::slot_type& signal_on_node_change_)
{
  LOG_DEBUG_ENTER;
  
  this->signal_on_node_change_.connect(signal_on_node_change_);
  
  LOG_DEBUG_EXIT;
}

void Manager::ConnectSlotModule(const SignalOnModuleChange::slot_type& slot_on_module_change)
{
  LOG_DEBUG_ENTER;
  
  this->signal_on_module_change_.connect(slot_on_module_change);
  
  LOG_DEBUG_EXIT;
}

void Manager::ConnectSlotModule(const SignalOnModuleMessage::slot_type& slot_on_module_message)
{
  LOG_DEBUG_ENTER;
  
  this->signal_on_module_message_.connect(slot_on_module_message);
  
  LOG_DEBUG_EXIT;
}

void Manager::SlotOnTimeoutHandler(timer::TimerId timer_id, bool repeat)
{
  LOG_DEBUG_ENTER;
  
  if (timer_id != this->timer_id_)
  {
    return;
  }
  
  for (NodeList::iterator it = this->nodes_.begin(); it != this->nodes_.end(); it++)
  {
    if (!it->second->CheckTimeout())
    {
      log::Info(log_module_, "Node %s has not sent anything in a long time, setting offline.", it->second->GetId().c_str());
      
      this->RemoveModules(it->second->GetId());
      
      storage::Manager::Instance()->FlushStore("NodeList");
      storage::Manager::Instance()->FlushStore("ModuleList");
    }
  }
  
  LOG_DEBUG_EXIT;
}

void Manager::SlotOnMessageHandler(broker::Message::Pointer message)
{
  LOG_DEBUG_ENTER;

  if (message->GetType() == broker::Message::CAN_MESSAGE)
  {
    //LOG.Debug("Message received");
    can::Message* payload = static_cast<can::Message*>(message->GetPayload().get());
    Node::Pointer node;
    
    if (payload->GetClassName() == "nmt")
    {
      if (payload->GetCommandName() == "Bios_Start")
      {
        node = this->GetNode(common::ToHex(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"])));
        node->Trigger(Node::EVENT_BIOS_START, payload->GetVariables());
      }
      else if (payload->GetCommandName() == "App_Start")
      {
        node = this->GetNode(common::ToHex(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"])));
        node->Trigger(Node::EVENT_APP_START, payload->GetVariables());
      }
      else if (payload->GetCommandName() == "Heartbeat")
      {
        node = this->GetNode(common::ToHex(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"])));
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
        node = this->GetNode(common::ToHex(boost::lexical_cast<unsigned int>(payload->GetVariables()["HardwareId"])));
        node->ResetTimeout();
        
        module->SetNodeId(node->GetId());
        
        if (this->GetNumberOfModules(node->GetId()) == boost::lexical_cast<unsigned int>(payload->GetVariables()["NumberOfModules"]))
        {
          node->Trigger(Node::EVENT_LIST_DONE, payload->GetVariables());
        }
        
        log::Info(log_module_, "Module %s is available on node %s.", module->GetFullId().c_str(), node->GetId().c_str());
        this->signal_on_module_change_(module->GetFullId(), true);
      }
      else if (module->GetNodeId() != "")
      {
        node = this->GetNode(module->GetNodeId());
        node->ResetTimeout();
        
        this->signal_on_module_message_(module->GetFullId(), payload->GetCommandName(), payload->GetVariables());
      }
    }
  }
  
  LOG_DEBUG_EXIT;
}

void Manager::SlotOnNewState(Node::Id node_id, Node::State current_state, Node::State target_state)
{
  LOG_DEBUG_ENTER;
  
  if (target_state != Node::STATE_NORM_INITIALIZED)
  {
    this->RemoveModules(node_id);
  }
  
  if (current_state != target_state)
  {
    this->signal_on_node_change_(node_id, (target_state == Node::STATE_NORM_INITIALIZED || target_state == Node::STATE_NORM_LIST || target_state == Node::STATE_NORM_ONLINE));
  }
  
  if (target_state == Node::STATE_BPGM_OFFLINE || target_state == Node::STATE_APGM_OFFLINE)
  {
    this->active_programming_node_id_ = node_id;
  }
  else if (target_state == Node::STATE_NORM_OFFLINE)
  {
    this->active_programming_node_id_ = "";
  }
    
  LOG_DEBUG_EXIT;
}

void Manager::SendMessageHandler(std::string full_id, std::string command, common::StringMap variables)
{
  LOG_DEBUG_ENTER;
  
  ModuleList::iterator it = this->modules_.find(full_id);
  
  if (it == this->modules_.end())
  {
    log::Warning(log_module_, "Can not send message to %s , it is not available.", full_id.c_str());
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
    log::Error(log_module_, "Failed to send message, %s", e.what());
  }
  
  LOG_DEBUG_EXIT;
}

void Manager::SendMessage(std::string full_id, std::string command, common::StringMap variables)
{
  LOG_DEBUG_ENTER;
  
  this->io_service_.post(boost::bind(&Manager::SendMessageHandler, this, full_id, command, variables));
  
  LOG_DEBUG_EXIT;
}

common::StringList Manager::GetAvailableModules()
{
  LOG_DEBUG_ENTER;
  
  common::StringList available_modules;
  
  for (ModuleList::iterator it = this->modules_.begin(); it != this->modules_.end(); it++)
  {
    available_modules.push_back(it->first);
  }
  
  LOG_DEBUG_EXIT;
  
  return available_modules;
}

common::StringList Manager::GetAvailableNodes()
{
  LOG_DEBUG_ENTER;
  
  common::StringList available_nodes;
  
  for (NodeList::iterator it = this->nodes_.begin(); it != this->nodes_.end(); it++)
  {
    std::string node = it->first;

    for (ModuleList::iterator it2 = this->modules_.begin(); it2 != this->modules_.end(); it2++)
    {
      if (it2->second->GetNodeId() == it->first)
      {
        node += "," + it2->first;
      }
    }
    
    available_nodes.push_back(node);
  }
  
  LOG_DEBUG_EXIT;
  
  return available_nodes;
}

bool Manager::ResetNode(Node::Id node_id)
{
  LOG_DEBUG_ENTER;
  
  NodeList::iterator it = this->nodes_.find(node_id);
  
  if (it == this->nodes_.end())
  {
      log::Error(log_module_, "Could not find node %s.", node_id.c_str());
      return false;
  }
  
  it->second->Reset();
  
  LOG_DEBUG_EXIT;
  
  return true;
}

bool Manager::ProgramNode(Node::Id node_id, bool is_bios, std::string filename)
{
  LOG_DEBUG_ENTER;

  NodeList::iterator it = this->nodes_.find(node_id);
  
  if (it == this->nodes_.end())
  {
    log::Error(log_module_, "Could not find node %s.", node_id.c_str());
    return false;
  }
  
  Code::Pointer code = Code::Pointer(new Code());
  if (!code->LoadIntelHexFile(filename))
  {
    log::Error(log_module_, "Could not parse %s.", filename.c_str());
    return false;
  }
  
  if (is_bios)
  {
    log::Debug(log_module_, "is_bios true");
    it->second->ProgramBios(code);
  }
  else
  {
    log::Debug(log_module_, "is_bios false");
    it->second->ProgramApplication(code);
  }
  
  LOG_DEBUG_EXIT;
  
  return true;
}

/* Program node with hexdata as argument */
bool Manager::ProgramNodeHex(Node::Id node_id, bool is_bios, std::string hex_data)
{
  LOG_DEBUG_ENTER;
  
  NodeList::iterator it = this->nodes_.find(node_id);
  
  if (it == this->nodes_.end())
  {
    log::Error(log_module_, "Could not find node %s.", node_id.c_str());
    return false;
  }
  
  Code::Pointer code = Code::Pointer(new Code());
  
  if (!code->ParseIntelHex(hex_data))
  {
    log::Error(log_module_, "Could not parse hexdata %s.", hex_data.c_str());
    return false;
  }
  
  if (is_bios)
  {
    log::Debug(log_module_, "is_bios true");
    it->second->ProgramBios(code);
  }
  else
  {
    log::Debug(log_module_, "is_bios false");
    it->second->ProgramApplication(code);
  }
  
  LOG_DEBUG_EXIT;
  
  return true;
}

Node::Information Manager::GetNodeInformation(Node::Id node_id)
{
  LOG_DEBUG_ENTER;
  
  NodeList::iterator it = this->nodes_.find(node_id);
  
  if (it == this->nodes_.end())
  {
    throw std::runtime_error("No such node exists, " + node_id);
  }
  
  LOG_DEBUG_EXIT;
  
  return it->second->GetInformation();
}

Node::Pointer Manager::GetNode(Node::Id node_id)
{
  LOG_DEBUG_ENTER;
  
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
  
  LOG_DEBUG_EXIT;
  
  return node;
}

Module::Pointer Manager::GetModule(Module::Id module_id, std::string module_name, std::string class_name)
{
  LOG_DEBUG_ENTER;
  
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
  
  LOG_DEBUG_EXIT;
  
  return module;
}

void Manager::RemoveModules(Node::Id node_id)
{
  LOG_DEBUG_ENTER;
  
  for (ModuleList::iterator it = this->modules_.begin(); it != this->modules_.end(); it++)
  {
    if (it->second->GetNodeId() == node_id)
    {
      log::Info(log_module_, "Module %s is unavailable.", it->second->GetFullId().c_str());
      this->signal_on_module_change_(it->second->GetFullId(), false);
      
      this->modules_.erase(it);
      break;
    }
  }
    
  LOG_DEBUG_EXIT;
}

unsigned int Manager::GetNumberOfModules(Node::Id node_id)
{
  LOG_DEBUG_ENTER;
  
  unsigned int number_of_modules = 0;
  
  for (ModuleList::iterator it = this->modules_.begin(); it != this->modules_.end(); it++)
  {
    if (it->second->GetNodeId() == node_id)
    {
      number_of_modules++;
    }
  }
  
  LOG_DEBUG_EXIT;
  
  return number_of_modules;
}

}; // namespace control
}; // namespace atom
