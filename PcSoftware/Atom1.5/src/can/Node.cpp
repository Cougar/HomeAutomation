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

#include "Node.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

#include "Manager.h"
#include "Message.h"
#include "broker/Manager.h"

namespace atom {
namespace can {

Node::TransitionList Node::transitions_;
    
Node::Node(Node::Id id) : LOG("can::Node")
{
    this->state_ = STATE_NORM_OFFLINE;
    this->id_ = id;
}

Node::~Node()
{

}

void Node::ConnectSlots(const SignalOnNewState::slot_type& slot_on_new_state)
{
    this->signal_on_new_state_.connect(slot_on_new_state);
}

Node::Id Node::GetId()
{
    return this->id_;
}

Node::State Node::GetState()
{
    return this->state_;
}

void Node::SetupStateMachine()
{
    // Normal flow
    Node::AddTransition(STATE_NORM_OFFLINE,          EVENT_BIOS_START,        STATE_NORM_ONLINE);
    Node::AddTransition(STATE_NORM_OFFLINE,          EVENT_APP_START,         STATE_NORM_LIST);
    Node::AddTransition(STATE_NORM_OFFLINE,          EVENT_HEARTBEAT,         STATE_NORM_LIST);
    Node::AddTransition(STATE_NORM_OFFLINE,          EVENT_PROGRAM_BIOS,      STATE_BPGM_OFFLINE);
    Node::AddTransition(STATE_NORM_OFFLINE,          EVENT_PROGRAM_APP,       STATE_APGM_OFFLINE);
    Node::AddTransition(STATE_NORM_OFFLINE,          EVENT_RESET,             STATE_NO_CHANGE);
    
    Node::AddTransition(STATE_NORM_ONLINE,           EVENT_BIOS_START,        STATE_NORM_ONLINE);
    Node::AddTransition(STATE_NORM_ONLINE,           EVENT_APP_START,         STATE_NORM_LIST);
    Node::AddTransition(STATE_NORM_ONLINE,           EVENT_HEARTBEAT,         STATE_NORM_LIST);
    Node::AddTransition(STATE_NORM_ONLINE,           EVENT_PROGRAM_BIOS,      STATE_BPGM_OFFLINE);
    Node::AddTransition(STATE_NORM_ONLINE,           EVENT_PROGRAM_APP,       STATE_APGM_OFFLINE);
    Node::AddTransition(STATE_NORM_ONLINE,           EVENT_RESET,             STATE_NORM_OFFLINE);
    
    Node::AddTransition(STATE_NORM_LIST,             EVENT_BIOS_START,        STATE_NORM_ONLINE);
    Node::AddTransition(STATE_NORM_LIST,             EVENT_APP_START,         STATE_NORM_LIST);
    Node::AddTransition(STATE_NORM_LIST,             EVENT_HEARTBEAT,         STATE_NO_CHANGE);
    Node::AddTransition(STATE_NORM_LIST,             EVENT_LIST_DONE,         STATE_NORM_INITIALIZED);
    Node::AddTransition(STATE_NORM_LIST,             EVENT_PROGRAM_BIOS,      STATE_BPGM_OFFLINE);
    Node::AddTransition(STATE_NORM_LIST,             EVENT_PROGRAM_APP,       STATE_APGM_OFFLINE);
    Node::AddTransition(STATE_NORM_LIST,             EVENT_RESET,             STATE_NORM_OFFLINE);
    
    Node::AddTransition(STATE_NORM_INITIALIZED,      EVENT_BIOS_START,        STATE_NORM_ONLINE);
    Node::AddTransition(STATE_NORM_INITIALIZED,      EVENT_APP_START,         STATE_NORM_LIST);
    Node::AddTransition(STATE_NORM_INITIALIZED,      EVENT_HEARTBEAT,         STATE_NO_CHANGE);
    Node::AddTransition(STATE_NORM_INITIALIZED,      EVENT_PROGRAM_BIOS,      STATE_BPGM_OFFLINE);
    Node::AddTransition(STATE_NORM_INITIALIZED,      EVENT_PROGRAM_APP,       STATE_APGM_OFFLINE);
    Node::AddTransition(STATE_NORM_INITIALIZED,      EVENT_RESET,             STATE_NORM_OFFLINE);
    
    
    // Program bios flow
    Node::AddTransition(STATE_BPGM_OFFLINE,          EVENT_BIOS_START,        STATE_BPGM_START);
    
    Node::AddTransition(STATE_BPGM_START,            EVENT_PGM_ACK,           STATE_BPGM_DATA);
    Node::AddTransition(STATE_BPGM_START,            EVENT_PGM_NACK,          STATE_NORM_OFFLINE); // Return to normal flow

    Node::AddTransition(STATE_BPGM_DATA,             EVENT_PGM_ACK,           STATE_BPGM_DATA); // If there is no more data to send STATE_PGM_END will be set instead
    Node::AddTransition(STATE_BPGM_DATA,             EVENT_PGM_NACK,          STATE_NORM_OFFLINE); // Return to normal flow
    
    Node::AddTransition(STATE_BPGM_END,              EVENT_PGM_ACK,           STATE_BPGM_COPY);
    Node::AddTransition(STATE_BPGM_END,              EVENT_PGM_NACK,          STATE_NORM_OFFLINE); // Return to normal flow
    
    Node::AddTransition(STATE_BPGM_COPY,             EVENT_BIOS_START,        STATE_NORM_ONLINE); // Return to normal flow
    
    
    // Program application flow
    Node::AddTransition(STATE_APGM_OFFLINE,          EVENT_BIOS_START,        STATE_APGM_START);
    
    Node::AddTransition(STATE_APGM_START,            EVENT_PGM_ACK,           STATE_APGM_DATA);
    Node::AddTransition(STATE_APGM_START,            EVENT_PGM_NACK,          STATE_NORM_OFFLINE); // Return to normal flow
    
    Node::AddTransition(STATE_APGM_DATA,             EVENT_PGM_ACK,           STATE_APGM_DATA); // If there is no more data to send STATE_PGM_END will be set instead
    Node::AddTransition(STATE_APGM_DATA,             EVENT_PGM_NACK,          STATE_NORM_OFFLINE); // Return to normal flow
    
    Node::AddTransition(STATE_APGM_END,              EVENT_PGM_ACK,           STATE_NORM_OFFLINE); // Return to normal flow
    Node::AddTransition(STATE_APGM_END,              EVENT_PGM_NACK,          STATE_NORM_OFFLINE); // Return to normal flow
}

void Node::AddTransition(Node::State current_state, Node::Event event, State target_state)
{
    Node::transitions_[current_state][event] = target_state;
}

Node::State Node::GetTransitionTarget(Node::Event event)
{
    Node::TransitionList::iterator it1 = Node::transitions_.find(this->state_);
    
    if (it1 == Node::transitions_.end())
    {
        LOG.Warning("No transitions found for current state " + boost::lexical_cast<std::string>(this->state_));
        return STATE_INVALID;
    }
    
    Node::Transition::iterator it2 = it1->second.find(event);
    
    if (it2 == it1->second.end())
    {
        LOG.Warning("No transitions found for current state " + boost::lexical_cast<std::string>(this->state_) + " for event " + boost::lexical_cast<std::string>(event));
        return STATE_INVALID;
    }
    
    return it2->second;
}

void Node::Trigger(Node::Event event, type::StringMap variables)
{
    this->ResetTimeout();
    
    State target_state = this->GetTransitionTarget(event);
    
    LOG.Debug("Trigger called: current state: " + boost::lexical_cast<std::string>(this->state_) + ", event: " + boost::lexical_cast<std::string>(event) + ", target state:"  + boost::lexical_cast<std::string>(target_state));
    
    if (target_state == STATE_INVALID)
    {
        this->SendReset();
        target_state = STATE_NORM_OFFLINE;
    }
    else if (target_state == STATE_NO_CHANGE)
    {
        return;
    }
    else if (target_state == STATE_NORM_LIST)
    {
        this->SendListRequest();
    }
    else if (target_state == STATE_NORM_OFFLINE)
    {
        this->SendReset();
    }
    else if (target_state == STATE_BPGM_OFFLINE || target_state == STATE_APGM_OFFLINE)
    {
        this->SendReset();
    }
    else if (target_state == STATE_BPGM_START || target_state == STATE_APGM_START)
    {
        // TODO: Send out pgm start
    }
    else if (target_state == STATE_BPGM_DATA || target_state == STATE_APGM_DATA)
    {
        // TODO: if more data send it
        // TODO: if no more data send pgm end and set target_state = STATE_APGM_END or target_state = STATE_BPGM_END
    }
    
    if (this->state_ != target_state)
    {
        this->signal_on_new_state_(this->id_, this->state_, target_state);
    }
    
    this->state_ = target_state;
}

bool Node::CheckTimeout()
{
    if (this->state_ == STATE_NORM_OFFLINE)
    {
        return true;
    }
    
    if (this->last_active_ + 10 < time(NULL))
    {
        this->state_ = STATE_NORM_OFFLINE;
        return false;
    }
    
    return true;
}

void Node::ResetTimeout()
{
    this->last_active_ = time(NULL);
}

void Node::SendReset()
{
    LOG.Debug("Sending node reset to " + type::ToHex(this->id_));
    Message* payload = new Message("nmt", "", "", 0, "Reset");
    payload->SetVariable("HardwareId", boost::lexical_cast<std::string>(this->id_));
    
    broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), Manager::Instance().get())));
}

void Node::SendListRequest()
{
    LOG.Debug("Sending module listing on node " + type::ToHex(this->id_));
    Message* payload = new Message("mnmt", "To_Owner", "", 0, "List");
    payload->SetVariable("HardwareId", boost::lexical_cast<std::string>(this->id_));
    
    broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), Manager::Instance().get())));
}

}; // namespace can
}; // namespace atom
