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

#ifndef TYPE_BITSET_H
#define TYPE_BITSET_H

#include <boost/shared_ptr.hpp>

#include "common.h"
#include "Byteset.h"

namespace atom {
namespace type {
        
class Bitset
{
public:
    typedef boost::shared_ptr<Bitset> Pointer;
    
    Bitset(unsigned int count);
    Bitset(const Byteset& set);
    virtual ~Bitset();
    
    int Set(unsigned int position);
    int Unset(unsigned int position);
    int Get(unsigned int position);
    
    unsigned long Read(unsigned int position, unsigned int length);
    
    unsigned int GetCount();
    unsigned char *bytes_;
private:
    
    unsigned int count_;
};

}; // namespace type
}; // namespace atom

#endif // TYPE_BITSET_H
