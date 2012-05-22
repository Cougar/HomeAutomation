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
#include <string>
#include <streambuf>
#include <stdexcept>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "common/common.h"
#include "common/log.h"

namespace atom {
namespace control {

#define MAX_BUFFER_SIZE  5200000

// Intel HEX record types
#define INTELHEX_RT_DATA 0x00
#define INTELHEX_RT_END  0x01

static const std::string log_module_ = "control::code";

Code::Code()
{
  LOG_DEBUG_ENTER;
  
  this->Reset();
  //this->data_.insert(this->data_.begin(), 0xFF, MAX_BUFFER_SIZE);
  
  LOG_DEBUG_EXIT;
}

Code::~Code()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
}

void Code::Reset()
{
  LOG_DEBUG_ENTER;
  
  this->address_lower_  = MAX_BUFFER_SIZE;
  this->address_upper_  = 0;
  this->is_valid_       = false;
  
  LOG_DEBUG_EXIT;
}

void Code::CalculateChecksum()
{
  LOG_DEBUG_ENTER;
  
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
  
  LOG_DEBUG_EXIT;
}

void Code::AddByte(unsigned char byte)
{
  LOG_DEBUG_ENTER;
  
  this->data_.push_back(byte);

  this->address_lower_  = 0;
  this->address_upper_  = this->data_.size();
  this->is_valid_       = true;

  this->CalculateChecksum();
  
  LOG_DEBUG_EXIT;
}

bool Code::LoadIntelHexFile(std::string filename)
{
  LOG_DEBUG_ENTER;
  
  std::ifstream file(filename.data());

  if (!file.is_open())
  {
    return false;
  }
  
  std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  
  file.close();
  
  bool result = this->ParseIntelHex(buffer);
  
  LOG_DEBUG_EXIT;

  return result;
}

bool Code::ParseIntelHex(std::string data)
{
  LOG_DEBUG_ENTER;
  
  common::StringList lines;

  boost::algorithm::split(lines, data, boost::is_any_of("\n"), boost::algorithm::token_compress_on);

  this->Reset();

  unsigned int byte_count;
  unsigned int addess;
  unsigned int record_type;

  for (unsigned int n = 0; n < lines.size(); n++)
  {
    log::Debug(log_module_, "line[" + boost::lexical_cast<std::string> (n) + "]=" + lines[n]);
    
    if (lines[n].length() >= 11 && lines[n][0] == ':')
    {
      //log::Debug(log_module_, "A:" + lines[n].substr(1, 2));
      byte_count = boost::lexical_cast<common::HexTo<unsigned int> >("0x" + lines[n].substr(1, 2)); //common::FromHex(lines[n].substr(1, 2));
      log::Debug(log_module_, "byte_count=" + boost::lexical_cast<std::string>(byte_count));
      
      //log::Debug(log_module_, "B:" + lines[n].substr(3, 4));
      addess = boost::lexical_cast<common::HexTo<unsigned int> >("0x" + lines[n].substr(3, 4)) & 0xFFFF; //common::FromHex(lines[n].substr(3, 4)) & 0xFFFF;
      log::Debug(log_module_, "addess=" +  boost::lexical_cast<std::string> (addess));
      
      record_type = boost::lexical_cast<common::HexTo<unsigned int> >("0x" + lines[n].substr(7, 2)); //common::FromHex(lines[n].substr(7, 2));

      switch (record_type)
      {
        case INTELHEX_RT_DATA:
        {
          this->address_lower_ = std::min(this->address_lower_, addess);
          this->address_upper_ = std::max(this->address_upper_, addess + byte_count - 1);

          for (unsigned int c = 0; c < byte_count; c++)
          {
            //log::Debug(log_module_, "D:" + lines[n].substr(c * 2 + 9, 2));
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

    log::Info(log_module_, "Successfully parsed Intel HEX file.");
  }

  LOG_DEBUG_EXIT;
  
  return this->is_valid_;
}

unsigned int Code::GetAddressLower()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
  
  return this->address_lower_;
}

unsigned int Code::GetAddressUpper()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
  
  return this->address_upper_;
}

unsigned char Code::GetByte(unsigned int address)
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
  
  return this->data_[address];
}

unsigned int Code::GetChecksum()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
  
  return this->checksum_;
}

unsigned int Code::GetLength()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
  
  return this->address_upper_ - this->address_lower_ + 1;
}

bool Code::IsValid()
{
  LOG_DEBUG_ENTER;
  LOG_DEBUG_EXIT;
  
  return this->is_valid_;
}

}; // namespace control
}; // namespace atom
