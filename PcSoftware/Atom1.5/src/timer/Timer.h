/*
 * 
 *    Copyright (C) 2010  Mattias Runge
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#ifndef TIMER_TIMER_H
#define TIMER_TIMER_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>

#include "types.h"

namespace atom {
namespace timer {

class Timer
{
public:
    typedef boost::shared_ptr<Timer> Pointer;
    
    typedef boost::signals2::signal<void(TimerId)> SignalOnTimeout;
    
    Timer(boost::asio::io_service& io_service, TimerId id, unsigned int timeout, bool repeat);
    Timer(boost::asio::io_service& io_service, TimerId id, std::string time);
    virtual ~Timer();

    void ConnectSlots(const SignalOnTimeout::slot_type& slot_on_timeout);
    
    TimerId GetId();
    bool GetRepeat();
    void Start();
    void Cancel();
    
private:
    boost::asio::deadline_timer instance_;
    TimerId id_;
    std::string time_;
    unsigned int timeout_;
    bool repeat_;
    
    SignalOnTimeout signal_on_timeout_;
    
    void TimeoutHandler(const boost::system::error_code& error);
};

}; // namespace timer
}; // namespace atom

#endif // TIMER_TIMER_H
