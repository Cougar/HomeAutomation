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
namespace broker {
   
Manager::Pointer Manager::instance_ = Manager::Pointer(new Manager());

Manager::Manager()
{
}

Manager::~Manager()
{
}

Manager::Pointer Manager::Instance()
{
    return Manager::instance_;
}

void Manager::Delete()
{
    Manager::instance_.reset();
}

void Manager::ConnectSlots(const SignalOnMessage::slot_type& slot_on_message)
{
    this->signal_on_message_.connect(slot_on_message);
}

void Manager::Post(Message::Pointer message)
{
    this->signal_on_message_(message);
}

}; // namespace broker
}; // namespace atom
