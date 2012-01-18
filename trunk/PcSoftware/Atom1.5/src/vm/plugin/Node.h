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

#ifndef VM_PLUGIN_NODE_H
#define VM_PLUGIN_NODE_H

#include <boost/shared_ptr.hpp>

#include "vm/Plugin.h"
#include "logging/Logger.h"
#include "common/common.h"
#include "control/Node.h"

namespace atom {
namespace vm {
namespace plugin {

class Node : public Plugin
{
public:
    typedef boost::shared_ptr<Node> Pointer;
    
    Node(boost::asio::io_service& io_service);
    virtual ~Node();
    
    void InitializeDone();
    void CallOutput(unsigned int request_id, std::string output);
    
private:
    static logging::Logger LOG;
    
    void SlotOnNodeChange(control::Node::Id node_id, bool available);
    
    void SlotOnNodeChangeHandler(control::Node::Id node_id, bool available);
    
    static Value Export_ResetNode(const v8::Arguments& args);
    static Value Export_GetAvailableNodes(const v8::Arguments& args);
    static Value Export_ProgramNode(const v8::Arguments& args);
    static Value Export_ProgramNodeHex(const v8::Arguments& args);
    static Value Export_GetNodeInformation(const v8::Arguments& args);
};

}; // namespace plugin
}; // namespace vm
}; // namespace atom

#endif // VM_PLUGIN_NODE_H
