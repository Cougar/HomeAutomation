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

#ifndef CONTROL_CODE_H
#define CONTROL_CODE_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "common/Byteset.h"
#include "logging/Logger.h"

namespace atom {
namespace control {

class Code
{
public:
    typedef boost::shared_ptr<Code> Pointer;
    
    Code();
    virtual ~Code();
    
    bool LoadIntelHexFile(std::string filename);
    bool ParseIntelHex(std::string data);
    
    unsigned int GetAddressLower();
    unsigned int GetAddressUpper();
    unsigned char GetByte(unsigned int address);
    unsigned int GetChecksum();
    unsigned int GetLength();
    bool IsValid();
    void Reset();
    void AddByte(unsigned char byte);
    
private:
    common::Byteset data_;
    bool is_valid_;
    unsigned int address_lower_;
    unsigned int address_upper_;
    unsigned int checksum_;
    
    logging::Logger LOG;
    
    
    void CalculateChecksum();
    
};

}; // namespace control
}; // namespace atom

#endif // CONTROL_CODE_H
