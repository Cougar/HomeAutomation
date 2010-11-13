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
#include "type/Bitset.h"

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
    
    std::string DecodeInt(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length);
    void EncodeInt(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value);
    std::string DecodeUint(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length);
    void EncodeUint(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value);
    std::string DecodeFloat(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length);
    void EncodeFloat(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value);
    std::string DecodeAscii(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length);
    void EncodeAscii(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value);
    std::string DecodeHexstring(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length);
    void EncodeHexstring(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value);
    
private:
    static Pointer instance_;
    
    xml::Node root_node_;
    
    Protocol();
        
    logging::Logger LOG;
};

}; // namespace can
}; // namespace atom

#endif // CAN_PROTOCOL_H
