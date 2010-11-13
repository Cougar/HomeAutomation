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

#ifndef BROKER_MANAGER_H
#define BROKER_MANAGER_H

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "Message.h"

namespace atom {
namespace broker {

class Manager
{
public:
    typedef boost::shared_ptr<Manager> Pointer;
    
    typedef boost::signals2::signal<void(Message::Pointer)> SignalOnMessage;
    
    virtual ~Manager();
    
    static Pointer Instance();
    static void Create();
    static void Delete();
    
    void ConnectSlots(const SignalOnMessage::slot_type& slot_on_message);
    
    void Post(Message::Pointer message);
    
private:
    static Pointer instance_;
    
    SignalOnMessage signal_on_message_;
    
    Manager();
};

}; // namespace broker
}; // namespace atom

#endif // BROKER_MANAGER_H
