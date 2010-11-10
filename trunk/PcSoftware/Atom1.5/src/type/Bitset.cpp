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

namespace atom {
namespace type {

Bitset::Bitset(unsigned int count)
{
    this->count_ = count;
    this->bytes_ = new unsigned char[(int)ceil(count / 8)];
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

unsigned long Bitset::Read(unsigned int position, unsigned int length)
{
    unsigned long value = 0;
    
    for (unsigned int index = 0; index < length; index++)
    {
        value = (value << 1) | this->Get(position + index);
    }
    
    return value;
}

int Bitset::Set(unsigned int position)
{
    if (position >= this->count_)
    {
        return -1;
    }
    
    this->bytes_[position / 8] |= (0x00000001 << (7 - position));

    return 0;
}

int Bitset::Unset(unsigned int position)
{
    if (position >= this->count_)
    {
        return -1;
    }
    
    this->bytes_[position / 8] &= ~(0x00000001 << (7 - position));
    
    return 0;
}

int Bitset::Get(unsigned int position)
{
    if (position >= this->count_)
    {
        return -1;
    }
    
    return (this->bytes_[position / 8] & (0x00000001 << (7 - position)) ? 1 : 0);
}

}; // namespace type
}; // namespace atom
