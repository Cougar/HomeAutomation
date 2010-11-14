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

#include <v8.h>
#include <stdio.h>

#include <boost/concept_check.hpp>
#include <boost/lexical_cast.hpp>

#include "vm/Manager.h"
#include "timer/Manager.h"

namespace atom {
namespace vm {
namespace plugin {

logging::Logger Timer::LOG("vm::plugin::Timer");

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

void Timer::SlotOnTimeoutHandler(timer::TimerId timer_id, bool repeat)
{
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    arguments->push_back(v8::Integer::New(timer_id));
    arguments->push_back(v8::Boolean::New(repeat));
    
    this->Call("Timer_OnTimeout", arguments);
}

Value Timer::Export_StartTimer(const v8::Arguments& args)
{
    return v8::Integer::New(timer::Manager::Instance()->Set(args[0]->Uint32Value(), args[1]->BooleanValue()));
}

Value Timer::Export_ClearTimer(const v8::Arguments& args)
{
    timer::Manager::Instance()->Cancel(args[0]->IsUint32());
    return v8::Undefined();
}

}; // namespace plugin
}; // namespace vm
}; // namespace atom