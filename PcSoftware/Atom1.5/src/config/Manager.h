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

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/program_options.hpp>

#include "type/common.h"

namespace atom {
namespace config {

class Manager
{
public:
    typedef boost::shared_ptr<Manager> Pointer;
    
    virtual ~Manager();
    
    static Pointer Instance();
    static void Create();
    static void Delete();
    
    bool Set(int argument_count, char **argument_vector);
    
    bool Exist(std::string name);
    
    std::string GetAsString(std::string name);
    type::StringList GetAsStringVector(std::string name);
    int GetAsInt(std::string name);
    
private:
    static Pointer instance_;
    
    boost::program_options::options_description command_line_;
    boost::program_options::options_description configuration_file_;
    boost::program_options::variables_map variable_map_;
    
    Manager();
};

}; // namespace config
}; // namespace atom

#endif // CONFIG_MANAGER_H
