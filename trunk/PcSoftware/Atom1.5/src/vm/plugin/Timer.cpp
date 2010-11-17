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

#include "Timer.h"

#include <v8-debug.h>
#include <stdio.h>

#include <boost/concept_check.hpp>
#include <boost/lexical_cast.hpp>

#include "vm/Manager.h"
#include "timer/Manager.h"

namespace atom {
namespace vm {
namespace plugin {

logging::Logger Timer::LOG("vm::plugin::Timer");

Timer::Timers Timer::timers_;

Timer::Timer()
{
    this->name_ = "timer";
    
    this->ImportFunction("Timer_OnTimeout");
    
    this->ExportFunction("StartTimer",      Timer::Export_StartTimer);
    this->ExportFunction("ClearTimer",      Timer::Export_ClearTimer);
}

Timer::~Timer()
{
    
}

void Timer::InitializeDone()
{
    atom::vm::Plugin::InitializeDone();
    
    timer::Manager::Instance()->ConnectSlots(timer::Manager::SignalOnTimeout::slot_type(&Timer::SlotOnTimeout, this, _1, _2).track(this->tracker_));
}

void Timer::SlotOnTimeout(timer::TimerId timer_id, bool repeat)
{
    if (!repeat)
    {
        Timers::iterator it = Timer::timers_.find(timer_id);
        
        if (it != Timer::timers_.end())
        {
            Timer::timers_.erase(it);
        }
    }
    
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    arguments->push_back(v8::Integer::New(timer_id));
    arguments->push_back(v8::Boolean::New(repeat));
    
    this->Call("Timer_OnTimeout", arguments);
}

Value Timer::Export_StartTimer(const v8::Arguments& args)
{
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 2)
    {
        LOG.Error("To few arguments.");
    }
    
    timer::TimerId timer_id = timer::Manager::Instance()->Set(args[0]->Uint32Value(), args[1]->BooleanValue());
    
    Timer::timers_.insert(timer_id);
    
    return v8::Integer::New(timer_id);
}

Value Timer::Export_ClearTimer(const v8::Arguments& args)
{
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error("To few arguments.");
    }
    
    Timers::iterator it = Timer::timers_.find(args[0]->IsUint32());
    
    if (it != Timer::timers_.end())
    {
        Timer::timers_.erase(it);
    }
    
    timer::Manager::Instance()->Cancel(args[0]->IsUint32());
    return v8::Undefined();
}

}; // namespace plugin
}; // namespace vm
}; // namespace atom