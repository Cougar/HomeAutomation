/*

    Copyright (C) 2010  Mattias Runge

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef CAN_NODE_H
#define CAN_NODE_H

#include <string>

#include <boost/shared_ptr.hpp>

namespace atom {
namespace can {

class Node
{
public:
    typedef boost::shared_ptr<Node> Pointer;
    typedef unsigned int Id;
    
    typedef enum
    {
        STATE_OFFLINE,
        STATE_ONLINE,
        STATE_PENDING,
        STATE_INITIALIZED
    } State;
    
    Node(Id id);
    virtual ~Node();
    
    Id GetId();
    State GetState();
    void SetState(State state);
    
private:
    Id id_;
    State state_;
};

}; // namespace can
}; // namespace atom

#endif // CAN_NODE_H
