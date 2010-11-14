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

#ifndef TIMER_SUBSCRIBER_H
#define TIMER_SUBSCRIBER_H

#include <boost/shared_ptr.hpp>

#include "common/IoService.h"
#include "types.h"

namespace atom {
namespace timer {

class Subscriber : virtual public common::IoService
{
public:
    typedef boost::shared_ptr<Subscriber> Pointer;
    
    Subscriber();
    virtual ~Subscriber();
    
protected:
    virtual void SlotOnTimeoutHandler(TimerId timer_id, bool repeat) = 0;
    
private:

    void SlotOnTimeout(TimerId timer_id, bool repeat);
};

}; // namespace timer
}; // namespace atom

#endif // TIMER_SUBSCRIBER_H
