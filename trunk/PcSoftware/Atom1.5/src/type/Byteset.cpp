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

#include "Byteset.h"

#include "string.h"

#include <boost/lexical_cast.hpp>

namespace atom {
namespace type {
 
Byteset::Byteset(unsigned int max_size)
{
    this->max_size_ = max_size;
    this->bytes_ = new unsigned char[this->max_size_];
    
    this->Clear();
}

Byteset::Byteset(const Byteset& set)
{
    this->max_size_ = set.size_;
    this->size_ = this->max_size_;
    this->bytes_ = new unsigned char[this->max_size_];

    memcpy(this->bytes_, set.bytes_, this->max_size_);
}

Byteset::Byteset(std::string str)
{
    this->max_size_ = str.size() + 1;
    this->size_ = this->max_size_;
    this->bytes_ = new unsigned char[this->max_size_];
    
    memcpy(this->bytes_, str.data(), this->max_size_);
}

Byteset::~Byteset()
{
    delete [] this->bytes_;
}

unsigned char* Byteset::Get() const
{
    return this->bytes_;
}

std::string Byteset::ToCharString()
{
    return std::string((char*)this->bytes_, this->size_ - 1);
}

std::string Byteset::ToDebugString()
{
    std::string debug_string;
    
    for (unsigned int n = 0; n < this->size_; n++)
    {
        debug_string += boost::lexical_cast<std::string>((unsigned int)this->bytes_[n]) + ",";
    }
    
    return debug_string;
}

unsigned char& Byteset::operator[](unsigned int index)
{
    return this->bytes_[index];
}

unsigned int Byteset::GetMaxSize() const
{
    return this->max_size_;
}

unsigned int Byteset::GetSize() const
{
    return this->size_;
}

void Byteset::SetSize(unsigned int size)
{
    this->size_ = size;
}

void Byteset::Append(unsigned char byte)
{
    this->bytes_[this->size_] = byte;
    this->size_++;
}

void Byteset::Clear()
{
    memset(this->bytes_, 0, this->max_size_);
    this->size_ = 0;
}
    
}; // namespace type
}; // namespace atom
