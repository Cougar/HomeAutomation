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

#include "Protocol.h"

#include <stdexcept>
#include <string.h>

#include <boost/lexical_cast.hpp>

namespace atom {
namespace can {

Protocol::Pointer Protocol::instance_;

Protocol::Protocol() : LOG("can::Protocol")
{
}

Protocol::~Protocol()
{
}

void Protocol::Create()
{
    Protocol::instance_ = Protocol::Pointer(new Protocol());
}

Protocol::Pointer Protocol::Instance()
{
    return Protocol::instance_;
}

void Protocol::Delete()
{
    Protocol::instance_.reset();
}
   
bool Protocol::Load(std::string filename)
{
    try
    {
        this->root_node_.LoadFile(filename);
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        return false;
    }
    
    LOG.Info("Protocol loaded successfully.");
    return true;
}

std::string Protocol::LookupClassName(unsigned int class_id)
{
    try
    {
        return this->root_node_.FindChild("classes").SelectChild("id", boost::lexical_cast<std::string>(class_id)).GetAttributeValue("name");
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such class_id found, " + boost::lexical_cast<std::string>(class_id));
    }
}

unsigned int Protocol::ResolveClassId(std::string class_name)
{
    try
    {
        return boost::lexical_cast<unsigned int>(this->root_node_.FindChild("classes").SelectChild("name", class_name).GetAttributeValue("id"));
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such class_name found, " + class_name);
    }
}

std::string Protocol::LookupCommandName(unsigned int command_id, std::string module_name)
{
    try
    {
        if (command_id >= 128)
        {
            return this->root_node_.FindChild("commands").SelectChild("id", boost::lexical_cast<std::string>(command_id), "module", module_name).GetAttributeValue("name");
        }
    
        return this->root_node_.FindChild("commands").SelectChild("id", boost::lexical_cast<std::string>(command_id)).GetAttributeValue("name");
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such command found, id = " + boost::lexical_cast<std::string>(command_id) + ", module = " + module_name);
    }
}

unsigned int Protocol::ResolveCommandId(std::string command_name, std::string module_name)
{
    try
    {
        return boost::lexical_cast<unsigned int >(this->root_node_.FindChild("commands").SelectChild("name", command_name, "module", module_name).GetAttributeValue("id"));
    }
    catch (std::runtime_error& e)
    {
    }
    
    try
    {
        return boost::lexical_cast<unsigned int >(this->root_node_.FindChild("commands").SelectChild("name", command_name).GetAttributeValue("id"));
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such command found, name = " + command_name + ", module = " + module_name);
    }
}

std::string Protocol::LookupNMTCommandName(unsigned int command_id)
{
    try
    {
        return this->root_node_.FindChild("nmt_messages").SelectChild("id", boost::lexical_cast<std::string>(command_id)).GetAttributeValue("name");
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such nmt command found, id = " + boost::lexical_cast<std::string>(command_id));
    }
}

unsigned int Protocol::ResolveNMTCommandId(std::string command_name)
{
    try
    {
        return boost::lexical_cast<unsigned int >(this->root_node_.FindChild("nmt_messages").SelectChild("name", command_name).GetAttributeValue("id"));
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such nmt command found, name = " + command_name);
    }
}

std::string Protocol::GetDefineId(std::string name, std::string group)
{
    try
    {
        return this->root_node_.FindChild("defines").SelectChild("name", name, "group", group).GetAttributeValue("id");
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such define found, name = " + name + ", group = " + group);
    }
}

std::string Protocol::LookupDirectionFlag(unsigned int direction_flag)
{
    try
    {
        return this->root_node_.FindChild("defines").SelectChild("id", boost::lexical_cast<std::string>(direction_flag), "group", "DirectionFlag").GetAttributeValue("name");
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such direction flag found, direction_flag = " + boost::lexical_cast<std::string>(direction_flag));
    }
}

unsigned int Protocol::ResolveDirectionFlag(std::string direction_name)
{
    try
    {
        return boost::lexical_cast<unsigned int>(this->root_node_.FindChild("defines").SelectChild("name", direction_name, "group", "DirectionFlag").GetAttributeValue("id"));
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such direction flag found, direction_name = " + direction_name);
    }
}

std::string Protocol::LookupModuleName(unsigned int module_id)
{
    try
    {
        return this->root_node_.FindChild("modules").SelectChild("id", boost::lexical_cast<std::string>(module_id)).GetAttributeValue("name");
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such module id found, " + boost::lexical_cast<std::string>(module_id));
    }
}

unsigned int Protocol::ResolveModuleId(std::string module_name)
{
    if (module_name == "")
    {
        return 0;
    }
    
    try
    {
        return boost::lexical_cast<unsigned int>(this->root_node_.FindChild("modules").SelectChild("name", module_name).GetAttributeValue("id"));
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such module name found, " + module_name);
    }
}

xml::Node::NodeList Protocol::GetCommandVariables(std::string command_name, std::string module_name)
{
    xml::Node node;
    
    try
    {
        node = this->root_node_.FindChild("commands").SelectChild("name", command_name, "module", module_name);
        
        try
        {
            return node.FindChild("variables").GetChildren();
        }
        catch (std::runtime_error& e)
        {
            return xml::Node::NodeList();
        }
    }
    catch (std::runtime_error& e)
    {
    }
  
    
    try
    {
        node = this->root_node_.FindChild("commands").SelectChild("name", command_name);
        
        try
        {
            return node.FindChild("variables").GetChildren();
        }
        catch (std::runtime_error& e)
        {
            return xml::Node::NodeList();
        }
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("Could not find variables for command, name = " + command_name + ", module = " + module_name);
    }
    
    return xml::Node::NodeList();
}

xml::Node::NodeList Protocol::GetNMTCommandVariables(std::string command_name)
{
    xml::Node node;
    
    try
    {
        node = this->root_node_.FindChild("nmt_messages").SelectChild("name", command_name);
        
        try
        {
            return node.FindChild("variables").GetChildren();
        }
        catch (std::runtime_error& e)
        {
            return xml::Node::NodeList();
        }
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("Could not find variables for nmt command, name = " + command_name);
    }
    
    return xml::Node::NodeList();
}

std::string Protocol::DecodeInt(common::Bitset& bitset, unsigned int start_bit, unsigned int bit_length)
{
    unsigned long raw_bit_value = bitset.Read(start_bit, bit_length);
    int raw_value = 0;
    
    memcpy(&raw_value, &raw_bit_value, sizeof(raw_value));
    
    return boost::lexical_cast<std::string>(raw_value);
}

void Protocol::EncodeInt(common::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value)
{
    unsigned long raw_bit_value = 0;
    int raw_value = boost::lexical_cast<int>(value);
    
    memcpy(&raw_bit_value, &raw_value, sizeof(raw_value));
    
    bitset.Write(start_bit, bit_length, raw_bit_value);
}

std::string Protocol::DecodeUint(common::Bitset& bitset, unsigned int start_bit, unsigned int bit_length)
{
    unsigned long raw_bit_value = bitset.Read(start_bit, bit_length);
    
    //LOG.Debug("DecodeUint: raw_bit_value=" + boost::lexical_cast<std::string>(raw_bit_value));
    
    return boost::lexical_cast<std::string>((unsigned int)raw_bit_value);
}

void Protocol::EncodeUint(common::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value)
{
    unsigned long raw_bit_value = boost::lexical_cast<unsigned int>(value);
    
    bitset.Write(start_bit, bit_length, raw_bit_value);
}

std::string Protocol::DecodeFloat(common::Bitset& bitset, unsigned int start_bit, unsigned int bit_length)
{
    float float_value = boost::lexical_cast<float>(this->DecodeInt(bitset, start_bit, bit_length)) / 64.0f;
    
    return boost::lexical_cast<std::string>(float_value);
}

void Protocol::EncodeFloat(common::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value)
{
    int int_value = boost::lexical_cast<float>(value) * 64.0f;
    
    this->EncodeInt(bitset, start_bit, bit_length, boost::lexical_cast<std::string>(int_value));
}

std::string Protocol::DecodeAscii(common::Bitset& bitset, unsigned int start_bit, unsigned int bit_length)
{
    std::string value;
    
    for (unsigned int n = 0; n < bit_length; n += 8)
    {
        unsigned long raw_value = bitset.Read(start_bit + n, 8);
        value += (char)raw_value;
    }
    
    return value;
}

void Protocol::EncodeAscii(common::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value)
{
    //LOG.Debug("EncodeAscii: " + value + ", length=" + boost::lexical_cast<std::string>(value.length()) + ", bit_length=" + boost::lexical_cast<std::string>(bit_length));
    
    for (unsigned int n = 0; n < bit_length; n += 8)
    {
        if (n / 8 >= value.length())
        {
            break;
        }
        
        //LOG.Debug(boost::lexical_cast<std::string>((unsigned int)value[n / 8]));
        
        bitset.Write(start_bit + n, 8, value[n / 8]);
    }
}

std::string Protocol::DecodeHexstring(common::Bitset& bitset, unsigned int start_bit, unsigned int bit_length)
{
    std::string value;
    char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    
    for (unsigned int n = 0; n < bit_length; n += 4)
    {
        unsigned long raw_bit_value = bitset.Read(start_bit + n, 4);
        
        if (raw_bit_value >= sizeof(hex))
        {
            value += '-';
        }
        else
        {
            value += hex[raw_bit_value];
        }
    }
    
    return value;
}

void Protocol::EncodeHexstring(common::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value)
{
    int ascii_value;
    
    for (unsigned int n = 0; n < bit_length; n += 4)
    {
        ascii_value = boost::lexical_cast<int>((int)std::toupper(value[n / 4]));
        
        if (48 <= ascii_value && ascii_value <= 57)
        {
            bitset.Write(start_bit + n, 4, ascii_value - 48);
        }
        else if (65 <= ascii_value && ascii_value <= 70)
        {
            bitset.Write(start_bit + n, 4, ascii_value - 65);
        }
        else
        {
            throw std::runtime_error("This is not an hex string, " + value);
        }
    }
}

}; // namespace can
}; // namespace atom
