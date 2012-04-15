/*
 * 
 *  Copyright (C) 2012  Mattias Runge
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

#ifndef VM_PLUGIN_MBB_H
#define VM_PLUGIN_MBB_H

#include <boost/shared_ptr.hpp>

#include "net/Subscriber.h"
#include "net/types.h"
#include "common/Byteset.h"

#include "vm/Plugin.h"

namespace atom {
namespace vm {
namespace plugin {

class Mbb : public Plugin
{
public:
    typedef boost::shared_ptr<Mbb> Pointer;
    
    Mbb(boost::asio::io_service& io_service, unsigned int port);
    virtual ~Mbb();
    
    void InitializeDone();
    void CallOutput(unsigned int request_id, std::string output);
    
private:
    net::ServerId server_id_;
    static net::ClientId current_client_id_;
    
    void SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state);
    void SlotOnNewData(net::ClientId client_id, net::ServerId server_id, common::Byteset data);
    
    void SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state);
    void SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, common::Byteset data);
    
    static Value Export_SendData(const v8::Arguments& args);
};
  
}; // namespace plugin
}; // namespace vm
}; // namespace atom

#endif // VM_PLUGIN_MBB_H