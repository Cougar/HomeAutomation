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

#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "logging/Logger.h"

#include "Store.h"

namespace atom {
namespace storage {
    
class Manager
{
public:
    typedef boost::shared_ptr<Manager> Pointer;
    
    virtual ~Manager();
    
    static Pointer Instance();
    static void Create();
    static void Delete();
    
    void SetRootPath(std::string root_path);
    
    Store::ParameterList& GetParameters(std::string store_name);
    std::string GetParameter(std::string store_name, std::string parameter_name);
    void SetParameter(std::string store_name, std::string parameter_name, std::string parameter_value);
    
private:
    typedef std::map<std::string, Store::Pointer> StoreList;
    
    static Pointer instance_;
    
    logging::Logger LOG;
    StoreList stores_;
    std::string root_path_;
    
    Manager();
};

}; // namespace storage
}; // namespace atom

#endif // STORAGE_MANAGER_H
