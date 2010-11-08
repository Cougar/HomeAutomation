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

#ifndef BROKER_MESSAGE_H
#define BROKER_MESSAGE_H

#include <boost/shared_ptr.hpp>

namespace atom {
namespace broker {

class Origin
{
};
    
class Message
{
public:
    typedef boost::shared_ptr<Message> Pointer;
    typedef boost::shared_ptr<void> PayloadPointer;
    
    typedef enum
    {
        CAN_MESSAGE,
        JS_COMMAND
    } Type;
    
    Message(Type type, PayloadPointer payload, Origin* origin);
    virtual ~Message();
    
    Type GetType();
    PayloadPointer GetPayload();
    bool TestIfOrigin(Origin* origin);
    
private:
    Type type_;
    PayloadPointer payload_;
    Origin* origin_;
    
};

}; // namespace broker
}; // namespace atom

#endif // BROKER_MESSAGE_H
