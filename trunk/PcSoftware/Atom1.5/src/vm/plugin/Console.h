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

#ifndef VM_PLUGIN_CONSOLE_H
#define VM_PLUGIN_CONSOLE_H

#include <boost/shared_ptr.hpp>

#include "logging/Logger.h"
#include "net/Subscriber.h"
#include "net/types.h"
#include "common/Byteset.h"

#include "vm/Plugin.h"

namespace atom {
namespace vm {
namespace plugin {

class Console : public Plugin
{
public:
    typedef boost::shared_ptr<Console> Pointer;
    
    Console(boost::asio::io_service& io_service, unsigned int port);
    virtual ~Console();
    
    void InitializeDone();
    void CallOutput(unsigned int request_id, std::string output);
    
private:
    net::SocketId server_id_;
    static net::SocketId current_client_id_;
    
    void SlotOnNewState(net::SocketId client_id, net::SocketId server_id, net::ClientState client_state);
    void SlotOnNewData(net::SocketId client_id, net::SocketId server_id, common::Byteset data);
    
    void SlotOnNewStateHandler(net::SocketId client_id, net::SocketId server_id, net::ClientState client_state);
    void SlotOnNewDataHandler(net::SocketId client_id, net::SocketId server_id, common::Byteset data);
    
    static logging::Logger LOG;
    
    static Value Export_PromptRequest(const v8::Arguments& args);
    static Value Export_AutoCompleteResponse(const v8::Arguments& args);
    static Value Export_LogToClient(const v8::Arguments& args);
    static Value Export_DisconnectClient(const v8::Arguments& args);
};
  
}; // namespace plugin
}; // namespace vm
}; // namespace atom

#endif // VM_PLUGIN_CONSOLE_H
