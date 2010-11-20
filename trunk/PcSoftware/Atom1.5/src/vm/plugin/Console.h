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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <boost/shared_ptr.hpp>

#include "logging/Logger.h"
#include "net/Subscriber.h"
#include "net/types.h"
#include "type/Byteset.h"

#include "vm/Plugin.h"

namespace atom {
namespace vm {
namespace plugin {

class Console : public Plugin
{
public:
    typedef boost::shared_ptr<Console> Pointer;
    
    Console(unsigned int port);
    virtual ~Console();
    
    void ExecutionResult(std::string response, unsigned int request_id);
    void InitializeDone();
    
private:
    net::ServerId server_id_;
    static type::StringList commands_;
    
    void SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state);
    void SlotOnNewData(net::ClientId client_id, net::ServerId server_id, type::Byteset data);
    
    static logging::Logger LOG;
    
    static Value Export_RegisterConsoleCommand(const v8::Arguments& args);
};
  
}; // namespace plugin
}; // namespace vm
}; // namespace atom

#endif // CONSOLE_H
