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

#include "Bitset.h"

#include <math.h>
#include <string.h>

#include <boost/lexical_cast.hpp>

namespace atom {
namespace common {

Bitset::Bitset(unsigned int count)
{
    unsigned int byte_size = (int)ceil(count / 8);
    
    this->count_ = count;
    this->bytes_ = new unsigned char[byte_size];
    
    memset(this->bytes_, 0, byte_size);
}

Bitset::Bitset(const Byteset& set)
{
    this->count_ = set.GetSize() * 8;
    this->bytes_ = new unsigned char[set.GetSize()];

    memcpy(this->bytes_, set.Get(), set.GetSize());
}

Bitset::~Bitset()
{
    delete [] this->bytes_;
}

unsigned int Bitset::GetCount()
{
    return this->count_;
}

unsigned char* Bitset::GetBytes() const
{
    return this->bytes_;
}

unsigned long Bitset::Read(unsigned int position, unsigned int length)
{
    unsigned long value = 0;
    
    for (unsigned int index = 0; index < length; index++)
    {
        value = (value << 1) | this->Get(position + index);
    }
    
    return value;
}

void Bitset::Write(unsigned int position, unsigned int length, long unsigned value)
{
    for (unsigned int index = 0; index < length; index++)
    {
        if (value & 0x01)
        {
            this->Set(position + (length - index - 1));
        }
        else
        {
            this->Unset(position + (length - index - 1));
        }
        
        value = (value >> 1);
    }
}

int Bitset::Set(unsigned int position)
{
    if (position >= this->count_)
    {
        return -1;
    }
    
    this->bytes_[position / 8] |= (0x00000001 << (7 - (position % 8)));

    return 0;
}

int Bitset::Unset(unsigned int position)
{
    if (position >= this->count_)
    {
        return -1;
    }
    
    this->bytes_[position / 8] &= ~(0x00000001 << (7 - (position % 8)));
    
    return 0;
}

int Bitset::Get(unsigned int position)
{
    if (position >= this->count_)
    {
        return -1;
    }
    
    return (this->bytes_[position / 8] & (0x00000001 << (7 - (position % 8))) ? 1 : 0);
}

std::string Bitset::ToDebugString()
{
    std::string debug_string;
    
    for (unsigned int n = 0; n < this->count_; n++)
    {
        debug_string += boost::lexical_cast<std::string>(this->Get(n));
    }
    
    return debug_string;
}

}; // namespace type
}; // namespace atom
