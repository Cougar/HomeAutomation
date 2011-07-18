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

#include <boost/lexical_cast.hpp>

#include "vm/Manager.h"
#include "timer/Manager.h"

namespace atom {
namespace vm {
namespace plugin {

logging::Logger Timer::LOG("vm::plugin::Timer");

Timer::Timers Timer::timers_;

Timer::Timer(boost::asio::io_service& io_service) : Plugin(io_service)
{
    this->name_ = "timer";
    
    this->ExportFunction("TimerExport_SetTimer",      Timer::Export_SetTimer);
    this->ExportFunction("TimerExport_SetAlarm",      Timer::Export_SetAlarm);
    this->ExportFunction("TimerExport_Cancel",        Timer::Export_Cancel);
    this->ExportFunction("TimerExport_Sleep",         Timer::Export_Sleep);
}

Timer::~Timer()
{
    
}

void Timer::InitializeDone()
{
    Plugin::InitializeDone();
    
    this->ImportFunction("Timer_OnTimeout");
    
    timer::Manager::Instance()->ConnectSlots(timer::Manager::SignalOnTimeout::slot_type(&Timer::SlotOnTimeout, this, _1, _2).track(this->tracker_));
}

void Timer::CallOutput(unsigned int request_id, std::string output)
{
    LOG.Info(output);
}

void Timer::SlotOnTimeout(timer::TimerId timer_id, bool repeat)
{
    this->io_service_.post(boost::bind(&Timer::SlotOnTimeoutHandler, this, timer_id, repeat));
}

void Timer::SlotOnTimeoutHandler(timer::TimerId timer_id, bool repeat)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
 
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
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
    
    this->Call( timer_id, "Timer_OnTimeout",arguments);
}

Value Timer::Export_SetAlarm(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    v8::String::AsciiValue time(args[0]);
    
    timer::TimerId timer_id = timer::Manager::Instance()->SetAlarm(*time);
    
    Timer::timers_.insert(timer_id);
    
    return handle_scope.Close(v8::Integer::New(timer_id));
}

Value Timer::Export_SetTimer(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 2)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    timer::TimerId timer_id = timer::Manager::Instance()->SetTimer(args[0]->Uint32Value(), args[1]->BooleanValue());
    
    Timer::timers_.insert(timer_id);
    
    return handle_scope.Close(v8::Integer::New(timer_id));
}

Value Timer::Export_Cancel(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    Timers::iterator it = Timer::timers_.find(args[0]->Uint32Value());
    
    if (it != Timer::timers_.end())
    {
        Timer::timers_.erase(it);
    }
    
    timer::Manager::Instance()->Cancel(args[0]->Uint32Value());
    return handle_scope.Close(v8::Undefined());
}

Value Timer::Export_Sleep(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    boost::this_thread::sleep(boost::posix_time::milliseconds(args[0]->Uint32Value()));
    
    return handle_scope.Close(v8::Undefined());
}

}; // namespace plugin
}; // namespace vm
}; // namespace atom
