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

#include "Node.h"

//#include <iostream>

#include <fstream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace atom {
namespace xml {

Node::Node()
{

}

Node::~Node()
{

}

void Node::LoadFile(std::string filename)
{
  std::ifstream file(filename.data());

  if (!file.is_open())
  {
    throw std::runtime_error("Could not find " + filename);
    return;
  }
  
  std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  
  file.close();
  
  this->Parse(buffer);
}

void Node::Parse(std::string data)
{
    char c;
    int position = 0;
    
    while ((unsigned int)position < data.length())
    {
        c = data.at(position);
        
        switch (c)
        {
            case '<':
            {
                position = this->ParseTag(data, position);
                break;
            }
        }
        
        position++;
    }
}

int Node::ParseTag(std::string data, int position)
{
    if (data.at(position) != '<')
    {
        throw std::runtime_error("Id not find < as expected. Character " + boost::lexical_cast<std::string>(position));
    }
    
    position++;
    
    int end_position = data.find('>', position);
    
    if (data.at(position) == '?' || data.at(position) == '!' || data.at(position) == '/')
    {
        return end_position;
    }
    
    std::string tag_string;
    
    try
    {
        tag_string = data.substr(position, end_position - position);
    }
    catch (std::out_of_range& e)
    {
        throw std::runtime_error(e.what());
    }
    
    boost::algorithm::trim(tag_string);
    boost::algorithm::trim_if(tag_string, boost::is_any_of("\n"));
    boost::algorithm::trim(tag_string);
    tag_string += " ";
    
    bool found_end = false;
    bool found_tag = false;
    std::string buffer;
    int pos = 0;
    char c;
    int s, e;
    
    while ((unsigned int)pos < tag_string.length())
    {
        c = tag_string.at(pos);
        
        switch (c)
        {
            case ' ':
            {
                if (!found_tag)
                {
                    this->tag_name_ = buffer;
                    buffer = "";
                    //std::cout << "Found tag: " << this->tag_name_ << std::endl;
                    found_tag = true;
                }
                break;
            }   
            case '/':
            {
                found_end = true;
                break;
            }
            case '=':
            {
                s = tag_string.find('"', pos) + 1;
                e = tag_string.find('"', s);
                
                try
                {
                    this->attributes_[buffer] = tag_string.substr(s, e - s);
                }
                catch (std::out_of_range& ex)
                {
                    throw std::runtime_error(ex.what());
                }

                pos = e;
                buffer = "";
                break;
            } 
            default:
            {
                buffer += c;
                break;
            }
        }
        
        pos++;
    }
    
    position = end_position;
    
    if (!found_end)
    {
        while ((unsigned int)position < data.length())
        {
            c = data.at(position);
            
            switch (c)
            {
                case '<':
                {
                    if (data.at(position+1) == '/')
                    {
                        position += 2;
                        end_position = data.find('>', position);
                        
                        try
                        {
                            if (this->tag_name_.compare(data.substr(position, end_position - position)) == 0)
                            {
                                //std::cout << "Found end tag: " << this->tag_name_ << std::endl;
                                return end_position;
                            }
                            else
                            {
                                throw std::runtime_error("Invalid end tag found. " + data.substr(position, end_position - position) + "::" + this->tag_name_);
                            }
                        }
                        catch (std::out_of_range& e)
                        {
                            throw std::runtime_error(e.what());
                        }
                        
                        
                    }
                    
                    Node sub_node;
                    position = sub_node.ParseTag(data, position);
                    this->children_.push_back(sub_node);
                    break;
                }
            }
            
            position++;
        }
    }
    
    return position;
}

Node Node::FindChild(std::string tag_name)
{
    for (unsigned int n = 0; n < this->children_.size(); n++)
    {
        if (this->children_[n].GetTagName().compare(tag_name) == 0)
        {
            return this->children_[n];
        }
    }
    
    throw std::runtime_error("No child with that tag name found, " + tag_name);
}

Node Node::SelectChild(std::string attribute_name, std::string attribute_value)
{
    for (unsigned int n = 0; n < this->children_.size(); n++)
    {
        if (this->children_[n].GetAttributeValue(attribute_name) == attribute_value)
        {
            return this->children_[n];
        }
    }
    
    throw std::runtime_error("No child with that attribute combination found, " + attribute_name + " = " + attribute_value);
}

Node Node::SelectChild(std::string attribute_name1, std::string attribute_value1, std::string attribute_name2, std::string attribute_value2)
{
    for (unsigned int n = 0; n < this->children_.size(); n++)
    {
        if (this->children_[n].GetAttributeValue(attribute_name1) == attribute_value1 && this->children_[n].GetAttributeValue(attribute_name2) == attribute_value2)
        {
            return this->children_[n];
        }
    }
    
    throw std::runtime_error("No child with that attribute combination found, " + attribute_name1 + " = " + attribute_value1 + " && " + attribute_name2 + " = " + attribute_value2);
}

std::string Node::GetAttributeValue(std::string attribute_name)
{
    AttributeList::const_iterator iterator = this->attributes_.find(attribute_name);
    
    if (iterator == this->attributes_.end())
    {
        throw std::runtime_error("No attribute with that name found, " + attribute_name);
    }
    
    return iterator->second;
}

Node::AttributeList Node::GetAttributes()
{
    return this->attributes_;
}

std::vector<Node> Node::GetChildren()
{
    return this->children_;
}

std::string Node::GetTagName()
{
    return this->tag_name_;
}

}; // namespace xml
}; // namespace atom
