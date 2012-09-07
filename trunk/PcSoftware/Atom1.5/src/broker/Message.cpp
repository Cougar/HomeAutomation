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

#include "Message.h"

namespace atom {
namespace broker {

Message::Message(Message::Type type, Message::PayloadPointer payload, Origin* origin)
{
    this->type_ = type;
    this->payload_ = payload;
    this->origin_ = origin;
}

Message::Message(Message::Type type, common::Byteset data, Origin* origin)
{
  this->type_ = type;
  this->data_ = data;
  this->origin_ = origin;
}


Message::~Message()
{
}

Message::Type Message::GetType()
{
    return this->type_;
}

Message::PayloadPointer Message::GetPayload()
{
    return this->payload_;
}

common::Byteset Message::GetRawData()
{
    return this->data_;
}


bool Message::TestIfOrigin(Origin* origin)
{
    return (this->origin_ == origin);
}

}; // namespace broker
}; // namespace atom
