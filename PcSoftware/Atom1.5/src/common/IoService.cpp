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

#include "IoService.h"

namespace atom {
namespace common {
    
IoService::IoService() : io_service_work_(io_service_)
{
    boost::thread thread(boost::bind(&boost::asio::io_service::run, &this->io_service_));
    this->thread_ = thread.move();
    
    this->tracker_ = TrackerPointer(new char);
}

IoService::~IoService()
{
    this->tracker_.reset();
    
    this->io_service_.stop();
    
    this->thread_.interrupt();
    this->thread_.join();
}
    
}; // namespace common
}; // namespace atom
