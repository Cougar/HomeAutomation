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

#include "Code.h"

#include <fstream>
#include <stdexcept>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "common/common.h"

namespace atom {
namespace control {

#define MAX_BUFFER_SIZE  5200000
    
// Intel HEX record types
#define INTELHEX_RT_DATA 0x00
#define INTELHEX_RT_END  0x01
    
Code::Code() : data_(MAX_BUFFER_SIZE), LOG("control::code")
{
    this->Reset();
}

Code::~Code()
{

}

void Code::Reset()
{
    this->address_lower_ = MAX_BUFFER_SIZE;
    this->address_upper_ = 0;
    this->data_.SetSize(0);
    this->data_.Fill(0xFF);
    this->is_valid_ = false;
}

void Code::CalculateChecksum()
{
    this->checksum_ = 0;
    
    for (unsigned int n = this->address_lower_; n <= this->address_upper_; n++)
    {
        this->checksum_ ^= this->data_[n];
        
        for (unsigned int c = 0; c < 8; c++)
        {
            if ((this->checksum_ & 1) != 0)
            {
                this->checksum_ = (this->checksum_ >> 1) ^ 0xA001;
            }
            else
            {
                this->checksum_ = (this->checksum_ >> 1);
            }
        }
    }
    
    this->checksum_ &= 0xFFFF;
}

void Code::AddByte(unsigned char byte)
{
    this->data_.Append(byte);
    
    this->address_lower_ = 0;
    this->address_upper_ = this->data_.GetSize() - 1;
    this->is_valid_ = true;
    
    this->CalculateChecksum();
}

bool Code::LoadIntelHexFile(std::string filename)
{
    std::ifstream file(filename.data());
    
    if (!file.is_open())
    {
        return false;
    }
    
    std::string buffer = "";
    std::string line;
    
    while (!file.eof())
    {
        std::getline(file, line);
        
        if (file.eof())
        {
            break;
        }
        
        buffer += line + "\n";
    }
    
    file.close();
    
    return this->ParseIntelHex(buffer);
}

bool Code::ParseIntelHex(std::string data)
{
    common::StringList lines;
    
    boost::algorithm::split(lines, data, boost::is_any_of("\n"), boost::algorithm::token_compress_on);
    
    this->Reset();
    
    unsigned int byte_count;
    unsigned int addess;
    unsigned int record_type;
    
    for (unsigned int n = 0; n < lines.size(); n++)
    {
        LOG.Debug("line[" + boost::lexical_cast<std::string>(n) + "]=" + lines[n]);
        if (lines[n].length() >= 11 && lines[n][0] == ':')
        {
            //LOG.Debug("A:" + lines[n].substr(1, 2));
            byte_count = boost::lexical_cast<common::HexTo<unsigned int> >("0x" + lines[n].substr(1, 2));//common::FromHex(lines[n].substr(1, 2));
            LOG.Debug("byte_count=" + boost::lexical_cast<std::string>(byte_count));
            //LOG.Debug("B:" + lines[n].substr(3, 4));
            addess = boost::lexical_cast<common::HexTo<unsigned int> >("0x" + lines[n].substr(3, 4)) & 0xFFFF;//common::FromHex(lines[n].substr(3, 4)) & 0xFFFF;
            LOG.Debug("addess=" +  boost::lexical_cast<std::string>(addess));
            record_type = boost::lexical_cast<common::HexTo<unsigned int> >("0x" + lines[n].substr(7, 2));//common::FromHex(lines[n].substr(7, 2));
            
            switch (record_type)
            {
                case INTELHEX_RT_DATA:
                {
                    this->address_lower_ = std::min(this->address_lower_, addess);
                    this->address_upper_ = std::max(this->address_upper_, addess + byte_count - 1);
                    
                    for (unsigned int c = 0; c < byte_count; c++)
                    {
                        //LOG.Debug("D:" + lines[n].substr(c * 2 + 9, 2));
                        this->data_[addess + c] = boost::lexical_cast<common::HexTo<unsigned int> >("0x" + lines[n].substr(c * 2 + 9, 2));
                        //this->data[addess + c] = (unsigned char)common::FromHex(lines[n].substr(c * 2 + 9, 2));
                    }
                    
                    break;
                }
                case INTELHEX_RT_END:
                {
                    this->is_valid_ = true;
                    break; 
                }   
            }
        }
    }

    if (this->is_valid_)
    {
       this->CalculateChecksum();
        
        LOG.Info("Successfully parsed Intel HEX file.");
    }
    
    return this->is_valid_;
}

unsigned int Code::GetAddressLower()
{
    return this->address_lower_;
}

unsigned int Code::GetAddressUpper()
{
    return this->address_upper_;
}

unsigned char Code::GetByte(unsigned int address)
{
    return this->data_[address];
}

unsigned int Code::GetChecksum()
{
    return this->checksum_;
}

unsigned int Code::GetLength()
{
    return this->address_upper_ - this->address_lower_ + 1;
}

bool Code::IsValid()
{
    return this->is_valid_;
}

}; // namespace control
}; // namespace atom
