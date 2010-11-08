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

#include "Manager.h"

namespace atom {
namespace timer {

Manager::Pointer Manager::instance_ = Manager::Pointer(new Manager());

Manager::Manager() : io_service_work_(io_service_)
{
    boost::thread thread(boost::bind(&boost::asio::io_service::run, &this->io_service_));
    this->thread_ = thread.move();
}

Manager::~Manager()
{
    this->mutex_timers_.lock();
    this->timers_.clear();
    this->mutex_timers_.unlock();
    
    this->thread_.interrupt();
    this->thread_.join();
}

Manager::Pointer Manager::Instance()
{
    return Manager::instance_;
}

void Manager::Delete()
{
    Manager::instance_.reset();
}

void Manager::ConnectSlots(const SignalOnTimeout::slot_type& slot_on_timeout)
{
    this->signal_on_timeout_.connect(slot_on_timeout);
}

void Manager::SlotOnTimeout(TimerId id)
{
    this->mutex_timers_.lock();
    
    TimerList::iterator it = this->timers_.find(id);
    
    if (it != this->timers_.end())
    {
        if (!it->second->GetRepeat())
        {
            this->timers_.erase(id);
        }
    }
    
    this->mutex_timers_.unlock();
    
    this->signal_on_timeout_(id);
}

TimerId Manager::Set(unsigned int timeout, bool repeat)
{
    this->mutex_timers_.lock();
    
    Timer::Pointer timer = Timer::Pointer(new Timer(this->io_service_, this->GetFreeId(), timeout, repeat));

    this->timers_[timer->GetId()] = timer;
    
    this->mutex_timers_.unlock();
    
    timer->ConnectSlots(Timer::SignalOnTimeout::slot_type(&Manager::SlotOnTimeout, this, _1).track(Manager::instance_));
    
    timer->Start();
    
    return timer->GetId();
}

void Manager::Cancel(TimerId id)
{
    this->io_service_.post(boost::bind(&Manager::CancelHandler, this, id));
}

void Manager::CancelHandler(TimerId id)
{
    this->mutex_timers_.lock();
    
    TimerList::iterator it = this->timers_.find(id);
    
    if (it != this->timers_.end())
    {
        it->second->Cancel();
    }
    
    this->mutex_timers_.unlock();
}

TimerId Manager::GetFreeId()
{
    TimerId id = 1;
        
    while (this->timers_.find(id) != this->timers_.end())
    {
        id++;
    }
        
    return id;
}

}; // namespace timer
}; // namespace atom
