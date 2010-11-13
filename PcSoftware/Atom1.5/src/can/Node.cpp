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

#include "Node.h"

#include "Message.h"
#include "broker/Manager.h"

namespace atom {
namespace can {
    
Node::Node(Node::Id id)
{
    this->state_ = STATE_OFFLINE;
    this->id_ = id;
}

Node::~Node()
{

}

Node::Id Node::GetId()
{
    return this->id_;
}

Node::State Node::GetState()
{
    return this->state_;
}

void Node::SetState(Node::State state)
{
    this->state_ = state;
}
    
}; // namespace can
}; // namespace atom
