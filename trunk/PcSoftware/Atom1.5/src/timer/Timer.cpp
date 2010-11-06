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

namespace atom {
namespace timer {

Timer::Timer(boost::asio::io_service& io_service, TimerId id, unsigned int timeout, bool repeat) : instance_(io_service)
{
    this->id_ = id;
    this->timeout_ = timeout;
    this->repeat_ = repeat;
}

Timer::~Timer()
{
    this->Cancel();
}

void Timer::ConnectSlots(const SignalOnTimeout::slot_type& slot_on_timeout)
{
    this->signal_on_timeout_.connect(slot_on_timeout);
}

void Timer::Start()
{
    this->instance_.expires_from_now(boost::posix_time::millisec(this->timeout_));
    
    this->instance_.async_wait(boost::bind(&Timer::TimeoutHandler,
                                           this,
                                           boost::asio::placeholders::error));
}

void Timer::Cancel()
{
    this->instance_.cancel();
}

void Timer::TimeoutHandler(const boost::system::error_code& error)
{
    if (this->repeat_)
    {
        this->Start();
    }
    
    this->signal_on_timeout_(this->id_);
}

TimerId Timer::GetId()
{
    return this->id_;
}

bool Timer::GetRepeat()
{
    return this->repeat_;
}

}; // namespace timer
}; // namespace atom
