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

#include <boost/lexical_cast.hpp>

#include "control/Manager.h"
#include "vm/Manager.h"

namespace atom {
namespace vm {
namespace plugin {

logging::Logger Node::LOG("vm::plugin::node");
    
Node::Node(boost::asio::io_service& io_service) : Plugin(io_service)
{
    this->name_ = "node";
    
    this->ExportFunction("NodeExport_ResetNode",         Node::Export_ResetNode);
    this->ExportFunction("NodeExport_GetAvailableNodes", Node::Export_GetAvailableNodes);
    this->ExportFunction("NodeExport_ProgramNode",       Node::Export_ProgramNode);
}

Node::~Node()
{
    this->tracker_.reset();
}

void Node::InitializeDone()
{
    Plugin::InitializeDone();
    
    this->ImportFunction("Node_OnChange");
    
    control::Manager::Instance()->ConnectSlotNode(control::Manager::SignalOnNodeChange::slot_type(&Node::SlotOnNodeChange, this, _1, _2).track(this->tracker_));
}

void Node::CallOutput(unsigned int request_id, std::string output)
{
    LOG.Info(output);
}

void Node::SlotOnNodeChange(control::Node::Id node_id, bool available)
{
    this->io_service_.post(boost::bind(&Node::SlotOnNodeChangeHandler, this, node_id, available));
}

void Node::SlotOnNodeChangeHandler(control::Node::Id node_id, bool available)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    ArgumentListPointer arguments = ArgumentListPointer(new ArgumentList);
    arguments->push_back(v8::String::New(node_id.data()));
    arguments->push_back(v8::Boolean::New(available));
    
    this->Call(0, "Node_OnChange", arguments);
}

Value Node::Export_ResetNode(const v8::Arguments& args)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return v8::Boolean::New(false);
    }
    
    v8::String::AsciiValue node_id(args[0]);
    
    return v8::Boolean::New(control::Manager::Instance()->ResetNode(*node_id));
}

Value Node::Export_GetAvailableNodes(const v8::Arguments& args)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    common::StringList available_nodes = control::Manager::Instance()->GetAvailableNodes();
    
    v8::Local<v8::Array> result = v8::Array::New(available_nodes.size());
    
    for (unsigned int n = 0; n < available_nodes.size(); n++)
    {
        LOG.Debug(boost::lexical_cast<std::string>(n) + ": " + available_nodes[n]);
        result->Set(n, v8::String::New(available_nodes[n].data()));
    }
    
    return result;
}

Value Node::Export_ProgramNode(const v8::Arguments& args)
{
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    
    //LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 3)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return v8::Boolean::New(false);
    }
    
    v8::String::AsciiValue node_id(args[0]);
    v8::String::AsciiValue filename(args[2]);
    
    return v8::Boolean::New(control::Manager::Instance()->ProgramNode(*node_id, args[1]->BooleanValue(), std::string(*filename)));
}
    
}; // namespace plugin
}; // namespace vm
}; // namespace atom
