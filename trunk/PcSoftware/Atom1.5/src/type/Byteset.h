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

#ifndef TYPE_BUFFER_H
#define TYPE_BUFFER_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "common.h"

namespace atom {
namespace type {

class Byteset
{
public:
    typedef boost::shared_ptr<Byteset> Pointer;
    
    Byteset(unsigned int max_size);
    Byteset(const Byteset& set);
    Byteset(std::string str);
    virtual ~Byteset();
    
    unsigned int GetMaxSize() const;
    unsigned char* Get() const;
    
    std::string ToCharString();
    
    unsigned char& operator[](unsigned int index);

    unsigned int GetSize() const;
    void SetSize(unsigned int size);
    
    void Append(unsigned char byte);
    
    void Clear();
    
private:
    unsigned char* bytes_;
    unsigned int max_size_;
    
    unsigned int size_;
    
};

}; // namespace type
}; // namespace atom

#endif // TYPE_BUFFER_H
