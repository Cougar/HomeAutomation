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

#include "common.h"

namespace atom {
namespace common {

std::string PadNumber(unsigned int number, unsigned int length)
{
    std::string padded_string = boost::lexical_cast<std::string>(number);
    
    while (padded_string.length() < length)
    {
        padded_string = "0" + padded_string;
    }
    
    return padded_string;
}
    
std::string ToHex(unsigned int value)
{
    char hex_string[11];
    
    snprintf(hex_string, sizeof(hex_string), "0x%08X", value);
    
    return std::string(hex_string);
    
}

unsigned int FromHex(std::string hex_chars)
{
    int value = 0;
    int ascii_value;
    
    for (int n = hex_chars.length() - 1; n >= 0; n--)
    {
        ascii_value = (int)hex_chars.data()[n];
        
        if (48 <= ascii_value && ascii_value <= 57)
        {
            value |= (ascii_value - 48) << n;
        }
        else if (65 <= ascii_value && ascii_value <= 70)
        {
            value |= (ascii_value - 65) << n;
        }
        else
        {
            throw std::runtime_error("This is not an hex string, hex_chars = " + hex_chars + ", ascii_value = " + boost::lexical_cast<std::string>(ascii_value) + ", n = " + boost::lexical_cast<std::string>(n) + ", length = " + boost::lexical_cast<std::string>(hex_chars.length()));
        }
    }
    
    return value;
}
    
}; // namespace type
}; // namespace atom
