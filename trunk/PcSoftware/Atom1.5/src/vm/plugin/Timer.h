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

#ifndef VM_PLUGIN_TIMER_H
#define VM_PLUGIN_TIMER_H

#include <set>

#include <boost/shared_ptr.hpp>

#include "vm/Plugin.h"
#include "logging/Logger.h"
#include "timer/types.h"

namespace atom {
namespace vm {
namespace plugin {

class Timer : public Plugin
{
public:
    typedef boost::shared_ptr<Timer> Pointer;
    
    Timer(boost::asio::io_service& io_service);
    virtual ~Timer();
    
    void InitializeDone();
    void CallOutput(unsigned int request_id, std::string output);
    
private:
    typedef std::set<timer::TimerId> Timers;
    static logging::Logger LOG;

    static Timers timers_;
    
    void SlotOnTimeout(timer::TimerId timer_id, bool repeat);
    
    void SlotOnTimeoutHandler(timer::TimerId timer_id, bool repeat);
    
    static Value Export_SetTimer(const v8::Arguments& args);
    static Value Export_SetAlarm(const v8::Arguments& args);
    static Value Export_Cancel(const v8::Arguments& args);
    static Value Export_Sleep(const v8::Arguments& args);
};

}; // namespace plugin
}; // namespace vm
}; // namespace atom

#endif // VM_PLUGIN_TIMER_H
