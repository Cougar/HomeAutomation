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
    
    LOG.Info("Protocol loaded successfully");
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












/*





void Protocol::makeNMTDataValid(std::string commandName, map<std::string, CanVariable> &data)
{
    std::vector<Node> commandNodes = xmlNode.findChild("nmt_messages").getChildren();
    
    for (int n = 0; n < commandNodes.size(); n++)
    {
        map<std::string, std::string> attributes = commandNodes[n].getAttributes();
        
        if (attributes["name"] == commandName)
        {
            Node varablesNode = commandNodes[n].findChild("variables");
            
            makeDataValid(varablesNode.getChildren(), data);
            
            return;
        }
    }
}

void Protocol::makeDataValid(int commandId, std::string moduleName, map<std::string, CanVariable> &data)
{
    std::vector<Node> commandNodes = xmlNode.findChild("commands").getChildren();
    
    for (int n = 0; n < commandNodes.size(); n++)
    {
        map<std::string, std::string> attributes = commandNodes[n].getAttributes();
        
        bool correct = false;
        
        if (commandId >= 128)
        {
            if (stoi(attributes["id"]) == commandId && attributes["module"] == moduleName)
            {
                correct = true;
            }
        }
        else if (stoi(attributes["id"]) == commandId)
        {
            correct = true;
        }
        
        if (correct)
        {
            Node varablesNode = commandNodes[n].findChild("variables");
            
            makeDataValid(varablesNode.getChildren(), data);
            
            return;
        }
    }
}

void Protocol::makeDataValid(std::vector<Node> variableNodes, map<std::string, CanVariable> &data)
{
    ///FIXME: Remove variables that do not exist in the xmlfile
    for (int c = 0; c < variableNodes.size(); c++)
    {
        map<std::string, std::string> attributes = variableNodes[c].getAttributes();
        
        if (data.find(attributes["name"]) != data.end())
        {
            data[attributes["name"]].setType(attributes["type"]);
            data[attributes["name"]].setUnit(attributes["unit"]);
            data[attributes["name"]].setBitLength(stoi(attributes["bit_length"]));
            data[attributes["name"]].setStartBit(stoi(attributes["start_bit"]));
            
            if (attributes["type"] == "enum")
            {
                std::vector<Node> values = variableNodes[c].getChildren();
                
                for (int k = 0; k < values.size(); k++)
                {
                    map<std::string, std::string> valueAttributes = values[k].getAttributes();
                    data[attributes["name"]].addEnumValue(valueAttributes["id"], valueAttributes["name"]);
                }
            }
        }
    }
}

std::string Protocol::translateDataToHex(int commandId, std::string moduleName, map<std::string, CanVariable> &data)
{
    makeDataValid(commandId, moduleName, data);
    
    return translateValidDataToHex(data);
}


map<std::string, CanVariable> Protocol::translateData(int commandId, std::string moduleName, std::string dataHex)
{
    std::vector<Node> commandNodes = xmlNode.findChild("commands").getChildren();
    
    for (int n = 0; n < commandNodes.size(); n++)
    {
        map<std::string, std::string> attributes = commandNodes[n].getAttributes();
        
        bool correct = false;
        
        if (commandId >= 128)
        {
            if (stoi(attributes["id"]) == commandId && attributes["module"] == moduleName)
            {
                correct = true;
            }
        }
        else if (stoi(attributes["id"]) == commandId)
        {
            correct = true;
        }
        
        if (correct)
        {
            Node varablesNode = commandNodes[n].findChild("variables");
            
            return translateData(varablesNode.getChildren(), dataHex);
        }
    }
    
    map<std::string, CanVariable> variables;
    return variables;
}

map<std::string, CanVariable> Protocol::translateNMTData(int commandId, std::string dataHex)
{
    std::vector<Node> commandNodes = xmlNode.findChild("nmt_messages").getChildren();
    
    for (int n = 0; n < commandNodes.size(); n++)
    {
        map<std::string, std::string> attributes = commandNodes[n].getAttributes();
        
        if (stoi(attributes["id"]) == commandId)
        {
            Node varablesNode = commandNodes[n].findChild("variables");
            
            return translateData(varablesNode.getChildren(), dataHex);
        }
    }
    
    map<std::string, CanVariable> variables;
    return variables;
}

std::string Protocol::translateNMTDataToHex(std::string commandName, map<std::string, CanVariable> &data)
{
    makeNMTDataValid(commandName, data);
    
    return translateValidDataToHex(data);
}


template <class T>
bool from_std::string(T& t, 
                const std::std::string& s, 
                std::ios_base& (*f)(std::ios_base&))
{
    std::istd::stringstream iss(s);
    return !(iss >> f >> t).fail();
}


std::string Protocol::translateValidDataToHex(map<std::string, CanVariable> &data)
{
    std::string bin = "0000000000000000000000000000000000000000000000000000000000000000";
    int highestBit = 0;
    
    for (map<std::string, CanVariable>::iterator iter = data.begin(); iter != data.end(); iter++)
    {
        if (iter->second.getType() == "uint")
        {
            if (iter->second.getStartBit() + iter->second.getBitLength() > highestBit)
                highestBit = iter->second.getStartBit() + iter->second.getBitLength();
            
            unsigned int a = stou(iter->second.getValue());
            bin.replace(iter->second.getStartBit(), iter->second.getBitLength(), uint2bin(a, iter->second.getBitLength()));
        }
        else if (iter->second.getType() == "int")
        {
            if (iter->second.getStartBit() + iter->second.getBitLength() > highestBit)
                highestBit = iter->second.getStartBit() + iter->second.getBitLength();
            
            unsigned int a = stou(iter->second.getValue());
            bin.replace(iter->second.getStartBit(), iter->second.getBitLength(), uint2bin(a, iter->second.getBitLength()));

        }
        else if (iter->second.getType() == "float")
        {
            if (iter->second.getStartBit() + iter->second.getBitLength() > highestBit)
                highestBit = iter->second.getStartBit() + iter->second.getBitLength();
            bin.replace(iter->second.getStartBit(), iter->second.getBitLength(), float2bin(stof(iter->second.getValue()), iter->second.getBitLength()));
            
        }
        else if (iter->second.getType() == "enum")
        {
            if (iter->second.getStartBit() + iter->second.getBitLength() > highestBit)
                highestBit = iter->second.getStartBit() + iter->second.getBitLength();
            
            std::string value = iter->second.getEnumIdValue();
            
            if (value == "")
            {
                throw new CanMessageException("Got enum that could not be converterd to a value. value = \"" + iter->second.getValue() + "\" enumString is \"" + iter->second.getEnumsString() + "\"\n");
            }
            else
            {
                bin.replace(iter->second.getStartBit(), iter->second.getBitLength(), uint2bin(stou(value), iter->second.getBitLength()));
            }
        }
        else if (iter->second.getType() == "ascii")
        {
            for (int n = 0; n < iter->second.getValue().length(); n++)
            {
                if (iter->second.getStartBit()+n*8 + 8 > highestBit)
                    highestBit = iter->second.getStartBit()+n*8 + 8;
                
                //bin.replace(iter->second.getStartBit()+n*8, 8, uint2bin((unsigned int)iter->second.getValue()[n], 8));
                    unsigned int temp = iter->second.getValue()[n];
                    if (temp > 0xff)
                    {
                        temp &= 0xff;
                    }
                    bin.replace(iter->second.getStartBit()+n*8, 8, uint2bin(temp, 8));
            }
        }
        else if (iter->second.getType() == "hexstd::string")
        {
            for (int n = 0; n < iter->second.getValue().length(); n++)
            {
                if (iter->second.getStartBit()+n*4 + 4 > highestBit)
                    highestBit = iter->second.getStartBit()+n*4 + 4;
                
                std::string character;
                character += iter->second.getValue()[n];
                
                bin.replace(iter->second.getStartBit()+n*4, 4, hex2bin(character));
            }
        }
    }
    
    try
    {
        while (highestBit%8 != 0)
        {
            highestBit++;
        }
        bin = bin.substr(0, highestBit);
    }
    catch (std::out_of_range& e)
    {
        cout << "DEBUG: translateValidDataToHex exception: " << e.what() << "\n";
        cout << "DEBUG: A bin=" << bin << " :: highestBit=" << highestBit << endl;
    }
    
    return bin2hex(bin);
}

map<std::string, CanVariable> Protocol::translateData(std::vector<Node> variableNodes, std::string dataHex)
{
    map<std::string, CanVariable> variables;
    std::string dataBin = hex2bin(dataHex);
    //dataBin = rpad(dataBin, 64, '0');
    
    for (int c = 0; c < variableNodes.size(); c++)
    {
        map<std::string, std::string> attributes = variableNodes[c].getAttributes();
        
        int bitStart = stoi(attributes["start_bit"]);
        int bitLength = stoi(attributes["bit_length"]);
        std::string bits;
        
        try
        {
            bits = dataBin.substr(bitStart, bitLength);
        }
        catch (std::out_of_range& e)
        {
            return variables;
        }
        
        CanVariable variable;
        
        variable.setName(attributes["name"]);
        variable.setType(attributes["type"]);
        variable.setUnit(attributes["unit"]);
        variable.setStartBit(bitStart);
        variable.setBitLength(bitLength);
        
        if (attributes["type"] == "uint")
        {
            variable.setValue(utos(bin2uint(bits)));
        }
        else if (attributes["type"] == "int")
        {
            variable.setValue(itos(bin2int(bits)));
        }
        else if (attributes["type"] == "float")
        {
            variable.setValue(ftos(bin2float(bits)));
        }
        else if (attributes["type"] == "enum")
        {
            std::vector<Node> values = variableNodes[c].getChildren();
            
            std::string value = utos(bin2uint(bits));
            
            for (int k = 0; k < values.size(); k++)
            {
                map<std::string, std::string> valueAttributes = values[k].getAttributes();
                variable.addEnumValue(valueAttributes["id"], valueAttributes["name"]);
                
                if (valueAttributes["id"] == value)
                {
                    variable.setValue(valueAttributes["name"]);
                }
            }
        }
        else if (attributes["type"] == "ascii")
        {
            std::string str;
            
            for (int k = 0; k < bits.length(); k += 8)
            {
                try
                {
                    str += (char)bin2uint(bits.substr(k, 8));
                }
                catch (std::out_of_range& e)
                {
                    cout << "DEBUG: TranslateData exception: " << e.what() << "\n";
                    cout << "DEBUG: B bits=" << bits << " :: k=" << k << endl;
                    continue;
                }
            }
            
            variable.setValue(str);
        }
        else if (attributes["type"] == "hexstd::string")
        {
            std::string str;
            
            for (int k = 0; k < bits.length(); k += 4)
            {
                try
                {
                    str += bin2hex(bits.substr(k, 4));
                }
                catch (std::out_of_range& e)
                {
                    cout << "DEBUG: TranslateData exception: " << e.what() << "\n";
                    cout << "DEBUG: C bits=" << bits << " :: k=" << k << endl;
                    continue;
                }
            }
            
            variable.setValue(str);
        }
        
        variables[attributes["name"]] = variable;
    }
    
    return variables;
}
   
   
   */
   
   
   
   
   
   
   
   
   
}; // namespace can
}; // namespace atom
