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

#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>

#include "common/IoService.h"
#include "logging/Logger.h"

#include "types.h"
#include "Timer.h"

namespace atom {
namespace timer {
    
class Manager : public virtual common::IoService
{
public:
    typedef boost::shared_ptr<Manager> Pointer;
    
    typedef boost::signals2::signal<void(TimerId, bool)> SignalOnTimeout;
    
    virtual ~Manager();
    
    static Pointer Instance();
    static void Create();
    static void Delete();
    
    void ConnectSlots(const SignalOnTimeout::slot_type& slot_on_timeout);

    TimerId SetAlarm(std::string time);
    TimerId SetTimer(unsigned int timeout, bool repeat);
    void Cancel(TimerId id);
    
private:
    typedef std::map<TimerId, Timer::Pointer> TimerList;
    
    static Pointer instance_;
    
    TimerList timers_;
    boost::mutex mutex_timers_;
    
    SignalOnTimeout signal_on_timeout_;
    
    logging::Logger LOG;
    
    Manager();
    
    TimerId GetFreeId();
    
    void CancelHandler(TimerId id);
    
    void SlotOnTimeout(TimerId id);
};

}; // namespace timer
}; // namespace atom

#endif // TIMER_MANAGER_H
