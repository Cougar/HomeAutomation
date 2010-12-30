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

#ifndef CONTROL_NODE_H
#define CONTROL_NODE_H

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include <time.h>

#include "common/common.h"
#include "logging/Logger.h"

#include "Code.h"
#include <boost/concept_check.hpp>

namespace atom {
namespace control {

class Node
{
public:
    typedef boost::shared_ptr<Node> Pointer;
    typedef std::string Id;
    
    typedef struct
    {
        bool valid_;
        unsigned int bios_version_;
        std::string device_type_;
        bool has_application_;
        time_t last_active_;
    } Information;
    
    typedef enum
    {
        STATE_INVALID,
        STATE_NO_CHANGE,
        STATE_NORM_OFFLINE,
        STATE_NORM_ONLINE,
        STATE_NORM_LIST,
        STATE_NORM_INITIALIZED,
        STATE_BPGM_OFFLINE,
        STATE_BPGM_START,
        STATE_BPGM_DATA,
        STATE_BPGM_END,
        STATE_BPGM_COPY,
        STATE_APGM_OFFLINE,
        STATE_APGM_START,
        STATE_APGM_DATA,
        STATE_APGM_END,
    } State;

    typedef enum
    {
        EVENT_BIOS_START,
        EVENT_APP_START,
        EVENT_HEARTBEAT,
        EVENT_PGM_ACK,
        EVENT_PGM_NACK,
        EVENT_LIST_DONE,
        EVENT_PROGRAM_BIOS,
        EVENT_PROGRAM_APP,
        EVENT_RESET
    } Event;
    
    typedef boost::signals2::signal<void(Id, State, State)> SignalOnNewState;
    
    Node(Id id);
    virtual ~Node();
    
    static void SetupStateMachine();
    
    void ConnectSlots(const SignalOnNewState::slot_type& slot_on_new_state);
    
    Id GetId();
    Information GetInformation();
    unsigned int GetBiosVersion();
    bool HasApplication();
    State GetState();

    void Trigger(Event event, common::StringMap variables);
    
    bool CheckTimeout();
    void ResetTimeout();
    
    void ProgramApplication(Code::Pointer code);
    void ProgramBios(Code::Pointer code);
    void Reset();
    
private:
    typedef std::map<Event, State> Transition;
    typedef std::map<State, Transition> TransitionList;
    
    Id id_;
    Information information_;    
    State state_;
    
    Code::Pointer code_;
    unsigned int start_offset_;
    unsigned int current_offset_;
    unsigned int expected_ack_data_;
    time_t program_start_time_;

    logging::Logger LOG;
    
    static TransitionList transitions_;
    static std::map<State, std::string> state_names_;
    static std::map<Event, std::string> event_names_;
    
    SignalOnNewState signal_on_new_state_;
    
    static void AddTransition(State current_state, Event event, State target_state);
    
    State GetTransitionTarget(Event event);
    void SendReset();
    void SendListRequest();
};

}; // namespace control
}; // namespace atom

#endif // CONTROL_NODE_H
