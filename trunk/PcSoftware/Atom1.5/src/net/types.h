/*
 *
 *    Copyright (C) 2010  Mattias Runge
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef NET_TYPES_H
#define NET_TYPES_H

#include <boost/array.hpp>

namespace atom {
namespace net {

typedef enum
{
  CLIENT_STATE_CONNECTED      = 0x00,
  CLIENT_STATE_DISCONNECTED   = 0x01,
  CLIENT_STATE_ACCEPTED       = 0x02
} ClientState;

typedef enum
{
    PROTOCOL_TCP              = 0x00,
    PROTOCOL_UDP              = 0x01,
    PROTOCOL_SERIAL           = 0x02
} Protocol;

typedef unsigned int SocketId;

}; // namespace net
}; // namespace atom

#endif // NET_TYPES_H
