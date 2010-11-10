/*
 * 
 *    Copyright (C) 2010  Mattias Runge
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#ifndef CAN_PROTOCOL_H
#define CAN_PROTOCOL_H

#include <boost/shared_ptr.hpp>

#include <string>

#include "xml/Node.h"
#include "logging/Logger.h"

namespace atom {
namespace can {

class Protocol
{
public:
    typedef boost::shared_ptr<Protocol> Pointer;
    
    virtual ~Protocol();
    
    static Pointer Instance();
    static void Delete();
    
    bool Load(std::string filename);
    
    
    std::string LookupClassName(unsigned int class_id);
    unsigned int ResolveClassId(std::string className);
    
    std::string LookupDirectionFlag(unsigned int direction_flag);
    unsigned int ResolveDirectionFlag(std::string direction_name);
    
    std::string LookupModuleName(unsigned int module_id);
    unsigned int ResolveModuleId(std::string module_name);
    
    std::string LookupCommandName(unsigned int command_id, std::string module_name);
    unsigned int ResolveCommandId(std::string command_name, std::string module_name);
    
    std::string GetDefineId(std::string name, std::string group);
    
    std::string LookupNMTCommandName(unsigned int command_id);
    unsigned int ResolveNMTCommandId(std::string command_name);

    xml::Node::NodeList GetCommandVariables(std::string command_name, std::string module_name);
    xml::Node::NodeList GetNMTCommandVariables(std::string command_name);
    
    
    /*
    
    std::map<std::string, CanVariable> TranslateData(unsigned int command_id, std::string module_name, std::string data_hex);
    std::string TranslateDataToHex(unsigned int command_id, std::string module_name, std::map<std::string, CanVariable> &data);
    
        void MakeDataValid(int command_id, std::string module_name, std::map<std::string, CanVariable> &data);
    
    
        std::map<std::string, CanVariable> TranslateNMTData(unsigned int command_id, std::string raw_hex_data);
        std::string TranslateNMTDataToHex(std::string command_name, std::map<std::string, CanVariable> &data);
    
        void MakeNMTDataValid(std::string command_name, std::map<std::string, CanVariable> &data);*/
    
private:
    static Pointer instance_;
    
    xml::Node root_node_;
    
    Protocol();
    /*
        std::string translateValidDataToHex(std::map<std::string, CanVariable> &data);
        std::map<std::string, CanVariable> translateData(xml::Node::NodeList variable_nodes, std::string data_hex);
    
        void makeDataValid(xml::Node::NodeList variable_nodes, std::map<std::string, CanVariable> &data);*/
        
    logging::Logger LOG;
};

}; // namespace can
}; // namespace atom

#endif // CAN_PROTOCOL_H
