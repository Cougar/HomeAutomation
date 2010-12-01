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

#ifndef STORAGE_STORE_H
#define STORAGE_STORE_H

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

namespace atom {
namespace storage {

class Store
{
public:
    typedef boost::shared_ptr<Store> Pointer;
    typedef std::map<std::string, std::string> ParameterList;
    
    typedef enum
    {
        FLUSH_INSTANT,
        FLUSH_MANUAL
    } FlushPolicy;
    
    Store(std::string filename);
    virtual ~Store();
    
    void Flush();
    void SetFlushPolicy(FlushPolicy flush_policy);
    
    ParameterList& GetParameters();
    std::string GetParameter(std::string name);
    void SetParameter(std::string name, std::string value);
    
private:
    ParameterList parameters_;    
    bool modified_;
    std::string filename_;
    FlushPolicy flush_policy;
    
};

}; // namespace storage
}; // namespace atom

#endif // STORAGE_STORE_H
