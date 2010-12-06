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

#include "Store.h"

#include <fstream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "common/common.h"

namespace atom {
namespace storage {

Store::Store(std::string filename)
{
    this->filename_ = filename;
    this->modified_ = false;
    this->flush_policy = FLUSH_INSTANT;
    
    std::ifstream file(filename.data());
    
    if (!file.is_open())
    {
        file.close();
        return;
    }
    
    std::string line;
    size_t pos;
    
    while (!file.eof())
    {
        std::getline(file, line);
        
        if (file.eof())
        {
            break;
        }
        
        pos = line.find_first_of("=");

        if (pos == std::string::npos)
        {
            continue;
        }
        
        this->parameters_[line.substr(0, pos)] = line.substr(pos + 1);
    }
    
    file.close();
}

Store::~Store()
{
    
}

void Store::SetFlushPolicy(Store::FlushPolicy flush_policy)
{
    this->flush_policy = flush_policy;
}

void Store::Flush()
{
    if (this->modified_)
    {
        std::ofstream file(this->filename_.data());

        if (!file.is_open())
        {
            throw std::runtime_error("Could not open " + this->filename_);
        }
        
        for (ParameterList::iterator it = this->parameters_.begin(); it != this->parameters_.end(); it++)
        {
            file << it->first << "=" << it->second << "\n";
        }
        
        file.close();
        this->modified_ = false;
    }
}

Store::ParameterList& Store::GetParameters()
{
    return this->parameters_;
}

std::string Store::GetParameter(std::string name)
{
    ParameterList::iterator it = this->parameters_.find(name);
    
    if (it == this->parameters_.end())
    {
        throw std::runtime_error("No such parameter");
    }
    
    return it->second;
}

void Store::SetParameter(std::string name, std::string value)
{
    if (this->parameters_[name] != value)
    {
        if (value == "")
        {
            this->parameters_.erase(name);
        }
        else
        {
            this->parameters_[name] = value;
        }   
            
        this->modified_ = true;
        
        if (this->flush_policy == FLUSH_INSTANT)
        {
            this->Flush();
        }
    }
}
   
}; // namespace storage
}; // namespace atom
