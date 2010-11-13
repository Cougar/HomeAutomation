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

Protocol::Pointer Protocol::instance_ = Protocol::Pointer(new Protocol());

Protocol::Protocol() : LOG("can::Protocol")
{
}

Protocol::~Protocol()
{
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
    if (module_id == 0)
    {
        return "";
    }
    
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
    try
    {
        return this->root_node_.FindChild("commands").SelectChild("name", command_name, "module", module_name).FindChild("variables").GetChildren();
    }
    catch (std::runtime_error& e)
    {
    }
    
    try
    {
        return this->root_node_.FindChild("commands").SelectChild("name", command_name).FindChild("variables").GetChildren();
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such command found, name = " + command_name + ", module = " + module_name);
    }
}

xml::Node::NodeList Protocol::GetNMTCommandVariables(std::string command_name)
{
    try
    {
        return this->root_node_.FindChild("nmt_messages").SelectChild("name", command_name).FindChild("variables").GetChildren();
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        throw std::runtime_error("No such nmt command found, name = " + command_name);
    }
}

std::string Protocol::DecodeInt(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length)
{
    unsigned long raw_bit_value = bitset.Read(start_bit, bit_length);
    int raw_value = 0;
    
    memcpy(&raw_value, &raw_bit_value, sizeof(raw_value));
    
    return boost::lexical_cast<std::string>(raw_value);
}

void Protocol::EncodeInt(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value)
{
    unsigned long raw_bit_value = 0;
    int raw_value = boost::lexical_cast<int>(value);
    
    memcpy(&raw_bit_value, &raw_value, sizeof(raw_value));
    
    bitset.Write(start_bit, bit_length, raw_bit_value);
}

std::string Protocol::DecodeUint(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length)
{
    unsigned long raw_bit_value = bitset.Read(start_bit, bit_length);
    
    return boost::lexical_cast<std::string>((unsigned int)raw_bit_value);
}

void Protocol::EncodeUint(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value)
{
    unsigned long raw_bit_value = boost::lexical_cast<unsigned int>(value);
    
    bitset.Write(start_bit, bit_length, raw_bit_value);
}

std::string Protocol::DecodeFloat(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length)
{
    float raw_value = 0;
    bool negative = false;;
    
    if (bitset.Get(start_bit))
    {
        negative = true;
    }
    
    for (int c = bit_length-1; c > 0; c--)
    {
        if (bitset.Get(start_bit + c) != negative)
        {
            raw_value += pow(2.0f, (int)(bit_length-1-c));
        }
    }
    
    raw_value /= 64.0f;
    
    if (negative)
    {
        raw_value = -raw_value;
    }
    
    return boost::lexical_cast<std::string>(raw_value);
}

void Protocol::EncodeFloat(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value)
{
 // TODO:
}

std::string Protocol::DecodeAscii(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length)
{
    std::string value;
    char c = 0;
    
    for (unsigned int p = 0; p < bit_length; p += 8)
    {
        unsigned long raw_bit_value = bitset.Read(start_bit + p, 8);
        memcpy(&c, &raw_bit_value, sizeof(c));
        value += c;
    }
    
    return value;
}

void Protocol::EncodeAscii(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value)
{
    // TODO
}

std::string Protocol::DecodeHexstring(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length)
{
    std::string value;
    char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    
    for (unsigned int p = 0; p < bit_length; p += 4)
    {
        unsigned long raw_bit_value = bitset.Read(start_bit + p, 4);
        
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

void Protocol::EncodeHexstring(type::Bitset& bitset, unsigned int start_bit, unsigned int bit_length, std::string value)
{
    // TODO:
}

}; // namespace can
}; // namespace atom
