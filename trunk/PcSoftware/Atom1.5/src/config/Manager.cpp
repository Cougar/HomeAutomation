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

#include "Manager.h"

#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

namespace atom {
namespace config {

Manager::Pointer Manager::instance_;

Manager::Manager() : command_line_("Command line options"), configuration_file_("Configuration file options")
{
    std::string default_config_file = "/etc/atom/atom.conf";
    
    if (!boost::filesystem::exists(default_config_file))
    {
        default_config_file = "atom.conf";
    }

    this->configuration_file_.add_options()
    ("MonitorPort",   boost::program_options::value<int>()->default_value(1201), "TCP port to open for monitor output")
    ("CommandPort",   boost::program_options::value<int>()->default_value(1202), "TCP port to open for command input")
    ("LogFile",       boost::program_options::value<std::string>(),              "File to log output to")
    ("LogLevel",      boost::program_options::value<int>()->default_value(4),    "Level of logging")
    ("ScriptPath",    boost::program_options::value<std::string>(),              "Path to where the scripts are")
    ("UserScriptPath",boost::program_options::value<std::string>(),              "Path to where the user scripts are")
    ("StoragePath",   boost::program_options::value<std::string>(),              "Path to where the storage files are")
    ("ProtocolFile",  boost::program_options::value<std::string>(),              "File to read the protocol form")
    ("CanNet",        boost::program_options::value<common::StringList>(),       "Information on where to locate the CAN networks")
    ("Legacy",                                                                   "Start legacy VM environment");

    this->command_line_.add_options()
    ("help,h",    "produce help message")
    ("daemon,d",  "start in daemon mode")
    ("file,f",    boost::program_options::value<std::string>()->default_value(default_config_file), "configuration file");
    
    this->command_line_.add(this->configuration_file_);
}

Manager::~Manager()
{
}

Manager::Pointer Manager::Instance()
{
    return Manager::instance_;
}

void Manager::Create()
{
    Manager::instance_ = Manager::Pointer(new Manager());
}

void Manager::Delete()
{
    Manager::instance_.reset();
}

bool Manager::Set(int argument_count, char** argument_vector)
{
    try
    {
        boost::program_options::store(boost::program_options::command_line_parser(argument_count, argument_vector).options(this->command_line_).run(), this->variable_map_);
    }
    catch (boost::program_options::unknown_option e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << this->command_line_ << std::endl;
        return false;
    }
    catch (boost::program_options::invalid_syntax e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << this->command_line_ << std::endl;
        return false;
    }
    
    std::ifstream ifs(this->GetAsString("file").data());
        
    if (!ifs.is_open())
    {
        std::cerr << "Error: Could not find " << this->GetAsString("file") << std::endl;
        return false;
    }
    
    try
    {
        boost::program_options::store(boost::program_options::parse_config_file(ifs, this->configuration_file_), this->variable_map_);
    }
    catch (boost::program_options::unknown_option e)
    {
        ifs.close();
        std::cerr << e.what() << " found in " << this->GetAsString("file") << std::endl;
        std::cout << this->configuration_file_ << std::endl;
        return false;
    }
    catch (boost::program_options::invalid_syntax e)
    {
        std::cerr << e.what() << " found in " << this->GetAsString("file") << std::endl;
        std::cout << this->command_line_ << std::endl;
        return false;
    }
        
    ifs.close();
    
    if (this->Exist("help"))
    {
        std::cout << this->command_line_ << std::endl;
        return false;
    }
    
    std::cout << "Using " << this->GetAsString("file") << " as configuration file" << std::endl;
    
    return true;
}

bool Manager::Exist(std::string name)
{
    return this->variable_map_.count(name) != 0;
}

int Manager::GetAsInt(std::string name)
{
    return this->variable_map_[name].as<int>();
}

std::string Manager::GetAsString(std::string name)
{
    return this->variable_map_[name].as<std::string>();
}

common::StringList Manager::GetAsStringVector(std::string name)
{
    return this->variable_map_[name].as<common::StringList>();
}


}; // namespace config
}; // namespace atom

