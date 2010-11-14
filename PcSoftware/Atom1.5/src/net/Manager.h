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

#ifndef NET_MANAGER_H
#define NET_MANAGER_H

#include <string>
#include <map>

#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>

#include "common/IoService.h"

#include "types.h"
#include "Client.h"
#include "TcpClient.h"
#include "UdpClient.h"
#include "SerialClient.h"

namespace atom {
namespace net {

class Manager : virtual common::IoService
{
public:
    typedef boost::shared_ptr<Manager> Pointer;

    typedef boost::signals2::signal<void(ClientId, ServerId, ClientState)> SignalOnNewState;
    typedef boost::signals2::signal<void(ClientId, ServerId, type::Byteset)> SignalOnNewData;

    virtual ~Manager();

    static Pointer Instance();
    static void Create();
    static void Delete();
    
    void ConnectSlots(const SignalOnNewState::slot_type& slot_on_new_state, const SignalOnNewData::slot_type& slot_on_new_data);
    
    ServerId StartServer(Protocol protocol, unsigned int port);
    ClientId Connect(Protocol protocol, std::string address, unsigned int port_or_baud);
    
    void StopServer(ServerId server_id);
    void Disconnect(ClientId client_id);
    
    void SendToAll(ServerId server_id, type::Byteset data);
    void SendTo(ClientId client_id, type::Byteset data);

private:
    typedef std::map<ClientId, Client::Pointer> ClientList;

    static Pointer instance_;
    
    ClientList clients_;
    
    SignalOnNewState signal_on_new_state_;
    SignalOnNewData signal_on_new_data_;
    
    Manager();
    
    void SlotOnNewState(ClientId client_id, ServerId server_id, ClientState client_state);
    void SlotOnNewData(ClientId client_id, ServerId server_id, type::Byteset data);
    
    void StopServerHandler(ServerId server_id);
    void DisconnectHandler(ClientId client_id);
    
    void SendToAllHandler(ServerId server_id, type::Byteset data);
    void SendToHandler(ClientId client_id, type::Byteset data);
    
    ClientId GetFreeClientId();
    ServerId GetFreeServerId();
};

}; // namespace net
}; // namespace atom

#endif // NET_MANAGER_H
