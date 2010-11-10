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

#ifndef XML_NODE_H
#define XML_NODE_H

#include <string>
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

namespace atom {
namespace xml {
        
class Node
{
public:
    typedef boost::shared_ptr<Node> Pointer;
    
    typedef std::map<std::string, std::string> AttributeList;
    typedef std::vector<Node> NodeList;
    
    Node();
    virtual ~Node();
    
    void LoadFile(std::string filename);
    
    std::string GetTagName();
    AttributeList GetAttributes();
    NodeList GetChildren();
    
    Node FindChild(std::string tag_name);
    Node SelectChild(std::string attribute_name, std::string attribute_value);
    Node SelectChild(std::string attribute_name1, std::string attribute_value1, std::string attribute_name2, std::string attribute_value2);
    std::string GetAttributeValue(std::string attribute_name);
    
    
private:
    std::string tag_name_;
    AttributeList attributes_;
    NodeList children_;
    
    void Parse(std::string data);
    int ParseTag(std::string data, int position);
};

}; // namespace xml
}; // namespace atom

#endif // XML_NODE_H
