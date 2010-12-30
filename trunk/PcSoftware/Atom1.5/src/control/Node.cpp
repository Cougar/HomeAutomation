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
#include "can/Message.h"
#include "broker/Manager.h"

namespace atom {
namespace control {

Node::TransitionList Node::transitions_;
std::map<Node::State, std::string> Node::state_names_;
std::map<Node::Event, std::string> Node::event_names_;
    
Node::Node(Node::Id id) : LOG("control::Node")
{
    this->state_ = STATE_NORM_OFFLINE;
    this->id_ = id;
    this->information_.has_application_ = false;
    this->information_.bios_version_ = 0;
    this->information_.device_type_ = "N/A";
    this->information_.last_active_ = 0;
    this->information_.valid_ = false;
    
    this->current_offset_ = 0;
    this->expected_ack_data_ = 0;
    this->program_start_time_ = 0;
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

Node::Information Node::GetInformation()
{
    return this->information_;
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
    
    Node::AddTransition(STATE_BPGM_COPY,             EVENT_BIOS_START,        STATE_APGM_OFFLINE); // Remove application after bios upgrade
        
    
    // Program application flow
    Node::AddTransition(STATE_APGM_OFFLINE,          EVENT_BIOS_START,        STATE_APGM_START);
    
    Node::AddTransition(STATE_APGM_START,            EVENT_PGM_ACK,           STATE_APGM_DATA);
    Node::AddTransition(STATE_APGM_START,            EVENT_PGM_NACK,          STATE_NORM_OFFLINE); // Return to normal flow
    
    Node::AddTransition(STATE_APGM_DATA,             EVENT_PGM_ACK,           STATE_APGM_DATA); // If there is no more data to send STATE_PGM_END will be set instead
    Node::AddTransition(STATE_APGM_DATA,             EVENT_PGM_NACK,          STATE_NORM_OFFLINE); // Return to normal flow
    
    Node::AddTransition(STATE_APGM_END,              EVENT_PGM_ACK,           STATE_NORM_OFFLINE); // Return to normal flow
    Node::AddTransition(STATE_APGM_END,              EVENT_PGM_NACK,          STATE_NORM_OFFLINE); // Return to normal flow
    
    
    Node::state_names_[STATE_INVALID] = "STATE_INVALID";
    Node::state_names_[STATE_NO_CHANGE] = "STATE_NO_CHANGE";
    Node::state_names_[STATE_NORM_OFFLINE] = "STATE_NORM_OFFLINE";
    Node::state_names_[STATE_NORM_ONLINE] = "STATE_NORM_ONLINE";
    Node::state_names_[STATE_NORM_LIST] = "STATE_NORM_LIST";
    Node::state_names_[STATE_NORM_INITIALIZED] = "STATE_NORM_INITIALIZED";
    Node::state_names_[STATE_BPGM_OFFLINE] = "STATE_BPGM_OFFLINE";
    Node::state_names_[STATE_BPGM_START] = "STATE_BPGM_START";
    Node::state_names_[STATE_BPGM_DATA] = "STATE_BPGM_DATA";
    Node::state_names_[STATE_BPGM_END] = "STATE_BPGM_END";
    Node::state_names_[STATE_BPGM_COPY] = "STATE_BPGM_COPY";
    Node::state_names_[STATE_APGM_OFFLINE] = "STATE_APGM_OFFLINE";
    Node::state_names_[STATE_APGM_START] = "STATE_APGM_START";
    Node::state_names_[STATE_APGM_DATA] = "STATE_APGM_DATA";
    Node::state_names_[STATE_APGM_END] = "STATE_APGM_END";
    
    Node::event_names_[EVENT_BIOS_START] = "EVENT_BIOS_START";
    Node::event_names_[EVENT_APP_START] = "EVENT_APP_START";
    Node::event_names_[EVENT_HEARTBEAT] = "EVENT_HEARTBEAT";
    Node::event_names_[EVENT_PGM_ACK] = "EVENT_PGM_ACK";
    Node::event_names_[EVENT_PGM_NACK] = "EVENT_PGM_NACK";
    Node::event_names_[EVENT_LIST_DONE] = "EVENT_LIST_DONE";
    Node::event_names_[EVENT_PROGRAM_BIOS] = "EVENT_PROGRAM_BIOS";
    Node::event_names_[EVENT_PROGRAM_APP] = "EVENT_PROGRAM_APP";
    Node::event_names_[EVENT_RESET] = "EVENT_RESET";
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

void Node::Trigger(Node::Event event, common::StringMap variables)
{
    this->ResetTimeout();
    
    State target_state = this->GetTransitionTarget(event);
    
    LOG.Debug("Trigger called on " + this->id_ + ", current state: " + Node::state_names_[this->state_] + ", event: " + Node::event_names_[event] + ", target state: "  + Node::state_names_[target_state]);
    
    if (event == EVENT_BIOS_START)
    {
        this->information_.has_application_ = boost::lexical_cast<int>(variables["HasApplication"]) > 0;
        this->information_.bios_version_ = boost::lexical_cast<unsigned int>(variables["BiosVersion"]);
        this->information_.device_type_ = variables["DeviceType"];
        this->information_.valid_ = true;
    }
    
    if (target_state == STATE_INVALID)
    {
        this->SendReset();
        target_state = STATE_NORM_OFFLINE;
    }
    else if (target_state == STATE_NO_CHANGE)
    {
        return;
    }
    
    if ((this->state_ == STATE_APGM_END || this->state_ == STATE_BPGM_END) && event == EVENT_PGM_ACK)
    {
        unsigned int checksum = boost::lexical_cast<unsigned int>(variables["Data"]);
        
        if (this->expected_ack_data_ != checksum)
        {
            LOG.Warning("Checksum received in ACK was incorrect got " + boost::lexical_cast<std::string>(checksum) + " expected " + boost::lexical_cast<std::string>(this->expected_ack_data_) + ", aborting...");
        }
        else if (this->state_ == STATE_BPGM_END)
        {
            float speed = (float)this->code_->GetLength() / (float)(time(NULL) - this->program_start_time_);
            
            LOG.Info("Data was successfully transferred to " + this->id_ + ", speed was " + boost::lexical_cast<std::string>(speed) + " B/s");
            
            LOG.Debug("Sending application programming copy to node " + this->id_);
            can::Message* payload = new can::Message("nmt", "", "", 0, "Pgm_Copy");
            
            unsigned int source0 = GET_LOW_BYTE_16(this->start_offset_);
            unsigned int source1 = GET_HIGH_BYTE_16(this->start_offset_);
            
            unsigned int destination0 = GET_LOW_BYTE_16(this->code_->GetAddressLower());
            unsigned int destination1 = GET_HIGH_BYTE_16(this->code_->GetAddressLower());
            
            unsigned int length0 = GET_LOW_BYTE_16(this->code_->GetLength());
            unsigned int length1 = GET_HIGH_BYTE_16(this->code_->GetLength());
            
            payload->SetVariable("Source0", boost::lexical_cast<std::string>(source0));
            payload->SetVariable("Source1", boost::lexical_cast<std::string>(source1));
            payload->SetVariable("Destination0", boost::lexical_cast<std::string>(destination0));
            payload->SetVariable("Destination1", boost::lexical_cast<std::string>(destination1));
            payload->SetVariable("Length0", boost::lexical_cast<std::string>(length0));
            payload->SetVariable("Length1", boost::lexical_cast<std::string>(length1));
            
            broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), Manager::Instance().get())));
        }
        else
        {
            float speed = (float)this->code_->GetLength() / (float)(time(NULL) - this->program_start_time_);
            
            LOG.Info("Programming was completed successfully on " + this->id_ + ", speed was " + boost::lexical_cast<std::string>(speed) + " B/s");
        }
    }
    
    if (target_state == STATE_NORM_LIST)
    {
        this->SendListRequest();
    }
    else if (target_state == STATE_BPGM_OFFLINE || target_state == STATE_APGM_OFFLINE)
    {
        if (event != EVENT_BIOS_START)
        {
            this->SendReset();
        }
    }
    else if (target_state == STATE_APGM_START || target_state == STATE_BPGM_START)
    {
        if (!this->code_->IsValid())
        {
            LOG.Error("Code is not valid, aborting...");
            target_state = STATE_NORM_OFFLINE;
        }
        else
        {
            LOG.Debug("Sending programming start to node " + this->id_);
            can::Message* payload = new can::Message("nmt", "", "", 0, "Pgm_Start");
            payload->SetVariable("HardwareId", boost::lexical_cast<std::string>(common::FromHex(this->id_)));
            
            this->start_offset_ = target_state == STATE_BPGM_START ? 0 : this->code_->GetAddressLower();
            
            unsigned int address0 = GET_LOW_BYTE_16(this->start_offset_);
            unsigned int address1 = GET_HIGH_BYTE_16(this->start_offset_);
            
            this->expected_ack_data_ = SWAP_BYTE_ORDER_16(this->start_offset_);
            
            payload->SetVariable("Address0", boost::lexical_cast<std::string>(address0));
            payload->SetVariable("Address1", boost::lexical_cast<std::string>(address1));
            payload->SetVariable("Address2", "0"); // Not used?
            payload->SetVariable("Address3", "0"); // Not used?
            
            this->current_offset_ = 0;
            
            broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), Manager::Instance().get())));
            
            this->program_start_time_ = time(NULL);
        }
    }
    else if (target_state == STATE_APGM_DATA || target_state == STATE_BPGM_DATA)
    {
        unsigned int offset = boost::lexical_cast<unsigned int>(variables["Data"]);
        
        if ( this->expected_ack_data_ != offset)
        {
            LOG.Warning("Offset received in ACK was incorrect got " + boost::lexical_cast<std::string>(offset) + " expected " + boost::lexical_cast<std::string>(this->expected_ack_data_) + ", aborting...");
            target_state = STATE_NORM_OFFLINE;
        }
        else if (this->current_offset_ >= this->code_->GetLength())
        {
            LOG.Debug("Sending programming end to node " + this->id_);
            
            unsigned int checksum = this->code_->GetChecksum();
            this->expected_ack_data_ = SWAP_BYTE_ORDER_16(checksum);
            
            can::Message* payload = new can::Message("nmt", "", "", 0, "Pgm_End");
            broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), Manager::Instance().get())));
            
            target_state = target_state == STATE_BPGM_DATA ? STATE_BPGM_END : STATE_APGM_END;
        }
        else
        {
            LOG.Debug("Sending programming data to node " + this->id_);
            can::Message* payload = new can::Message("nmt", "", "", 0, "Pgm_Data_48");
            
            unsigned int offset0 = GET_LOW_BYTE_16(this->start_offset_ + this->current_offset_);
            unsigned int offset1 = GET_HIGH_BYTE_16(this->start_offset_ + this->current_offset_);
            
            this->expected_ack_data_ = SWAP_BYTE_ORDER_16(this->start_offset_ + this->current_offset_);
            
            payload->SetVariable("Offset0", boost::lexical_cast<std::string>(offset0));
            payload->SetVariable("Offset1", boost::lexical_cast<std::string>(offset1));
            
            for (int n = 0; (this->current_offset_ < this->code_->GetLength()) && (n < 6); n++)
            {
                //LOG.Debug("Data" + boost::lexical_cast<std::string>(n) + "=" +  boost::lexical_cast<std::string>((unsigned int)this->code_->GetByte(this->code_->GetAddressLower() + this->current_offset_)));
                payload->SetVariable("Data" + boost::lexical_cast<std::string>(n), boost::lexical_cast<std::string>((unsigned int)this->code_->GetByte(this->code_->GetAddressLower() + this->current_offset_)));
                this->current_offset_++;
            }
            
            broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), Manager::Instance().get())));
        }
    }
    else if (target_state == STATE_BPGM_COPY)
    {
        LOG.Debug("Preparing to send null application to node " + this->id_);
        
        this->code_->Reset();
        
        this->code_->AddByte(0xFF);
        this->code_->AddByte(0xFF);
    }
    else if (target_state == STATE_NORM_OFFLINE)
    {
        this->information_.valid_ = false;
        this->SendReset();
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
    
    if (this->code_.use_count() == 0 && this->information_.last_active_ + 10 < time(NULL))
    {
        this->state_ = STATE_NORM_OFFLINE;
        return false;
    }
    
    return true;
}

void Node::ResetTimeout()
{
    this->information_.last_active_ = time(NULL);
}

void Node::Reset()
{
    common::StringMap variables;
    
    this->Trigger(EVENT_RESET, variables);
}

void Node::ProgramApplication(Code::Pointer code)
{
    this->code_ = code;
    
    common::StringMap variables;
    
    this->Trigger(EVENT_PROGRAM_APP, variables);
}

void Node::ProgramBios(Code::Pointer code)
{
    this->code_ = code;
    
    common::StringMap variables;
    
    this->Trigger(EVENT_PROGRAM_BIOS, variables);
}

void Node::SendReset()
{
    LOG.Info("Sending node reset to " + this->id_);
    can::Message* payload = new can::Message("nmt", "", "", 0, "Reset");
    payload->SetVariable("HardwareId", boost::lexical_cast<std::string>(common::FromHex(this->id_)));
    
    broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), Manager::Instance().get())));
}

void Node::SendListRequest()
{
    LOG.Debug("Sending module listing on node " + this->id_);
    can::Message* payload = new can::Message("mnmt", "To_Owner", "", 0, "List");
    payload->SetVariable("HardwareId", boost::lexical_cast<std::string>(common::FromHex(this->id_)));
    
    broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), Manager::Instance().get())));
}

}; // namespace control
}; // namespace atom
