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

#ifndef TYPE_COMMON_H
#define TYPE_COMMON_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include <stdio.h>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

namespace atom {
namespace common {

typedef boost::shared_ptr<unsigned char> BytePointer;
typedef std::vector<std::string> StringList;
typedef std::map<std::string, std::string> StringMap;

#define SWAP_BYTE_ORDER_16(x) (((x << 8) | (x >> 8)) & 0xFFFF)
#define GET_HIGH_BYTE_16(x)   ((x >> 8) & 0xFF);
#define GET_LOW_BYTE_16(x)    (x & 0xFF)

template <typename ElemT>
struct HexTo {
    ElemT value;
    operator ElemT() const {return value;}
    friend std::istream& operator>>(std::istream& in, HexTo& out) {
        in >> std::hex >> out.value;
        return in;
    }
};

std::string PadNumber(unsigned int number, unsigned int length);
unsigned int FromHex(std::string hex_chars);
std::string ToHex(unsigned int value);
    
}; // namespace type
}; // namespace atom

#endif // TYPE_COMMON_H