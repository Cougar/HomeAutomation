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

#include <stdio.h>

#include <boost/shared_ptr.hpp>

namespace atom {
namespace type {

typedef boost::shared_ptr<unsigned char> BytePointer;
typedef std::vector<std::string> StringList;
typedef std::map<std::string, std::string> StringMap;
/*
template <typename T>
struct HexTo
{
    T value;
    
    operator T() const
    {
        return value;
    }
    
    friend std::istream& operator>>(std::istream& in, HexTo& out)
    {
        in >> std::hex >> out.value;
        return in;
    }
};
*/
inline std::string ToHex(unsigned int value)
{
    char hex_string[11];
    
    snprintf(hex_string, sizeof(hex_string), "0x%08X", value);
    
    return std::string(hex_string);
    
}
    
}; // namespace type
}; // namespace atom

#endif // TYPE_COMMON_H