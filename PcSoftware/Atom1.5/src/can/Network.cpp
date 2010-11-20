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

#include "Network.h"

#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "net/Manager.h"

#include "broker/Manager.h"

#include "Protocol.h"
#include "Message.h"

#include "type/Bitset.h"
#include "type/common.h"

namespace atom {
namespace can {

enum
{
    PACKET_START = 253,
    PACKET_END   = 250,
    PACKET_PING  = 251
};
    
Network::Network(std::string address): broker::Subscriber(false), LOG("can::Network"), buffer_(2048)
{
    this->address_ = address;
    this->client_id_ = 0;
       
    // Examples of address
    // udp:192.168.1.250:1100
    // serial:/dev/ttyUSB0:38400
    
    type::StringList parts;
    boost::algorithm::split(parts, address, boost::is_any_of(":"), boost::algorithm::token_compress_off);
    
    if (parts.size() < 3)
    {
        LOG.Error("Malformed address string: " + address);
        return;
    }
    
    boost::algorithm::to_lower(parts[0]);
    
    if (parts[0] == "udp")
    {
        this->protocol_ = net::PROTOCOL_UDP;
    }
    else if (parts[0] == "serial")
    {
        this->protocol_ = net::PROTOCOL_SERIAL;
    }
    else
    {
        LOG.Error("Unknown protocol, only support udp and serial, got " + parts[0]);
        return;
    }
    
    this->address_ = parts[1];
    this->port_or_baud_ = boost::lexical_cast<unsigned int>(parts[2]);
    
    try
    {
        this->client_id_ = net::Manager::Instance()->Connect(this->protocol_, this->address_, this->port_or_baud_);
        LOG.Info("Connected to " + address);
        
        LOG.Info("Sending ping...");

        type::Byteset buffer(1);
        buffer[0] = PACKET_PING;
        
        net::Manager::Instance()->SendTo(this->client_id_, buffer);
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
    }
}

Network::~Network()
{
    net::Manager::Instance()->Disconnect(this->client_id_);
}

void Network::SlotOnTimeout(timer::TimerId timer_id)
{
    if (timer_id == this->timer_id_)
    {
        this->io_service_.post(boost::bind(&Network::SlotOnTimeoutHandler, this, timer_id));
    }
}

void Network::SlotOnMessageHandler(broker::Message::Pointer message)
{
    if (message->GetType() == broker::Message::CAN_MESSAGE)
    {
        Message* payload = static_cast<Message*>(message->GetPayload().get());
        type::Byteset data(17);
        
        data[0] = PACKET_START;
        
        unsigned int class_id = Protocol::Instance()->ResolveClassId(payload->GetClassName());
        data[4] = class_id << 1;

        if (payload->GetClassName() == "nmt")
        {
            unsigned int command_id = Protocol::Instance()->ResolveNMTCommandId(payload->GetCommandName());
            data[3] = command_id;
        }
        else
        {
            unsigned int direction_flag = Protocol::Instance()->ResolveDirectionFlag(payload->GetDirectionName());
            data[4] |= (direction_flag & 0x01);
            
            unsigned int module_id = Protocol::Instance()->ResolveModuleId(payload->GetModuleName());
            data[3] = module_id;
            
            data[2] = payload->GetId();
            
            unsigned int command_id = Protocol::Instance()->ResolveCommandId(payload->GetCommandName(), payload->GetModuleName());
            data[1] = command_id;
        }
        
        unsigned int highest_bit = 0;
        type::Bitset databits(64);
        
        xml::Node::NodeList variable_nodes;
        unsigned int start_bit;
        unsigned int bit_length;
        std::string type;
        std::string value;
        
        if (payload->GetClassName() == "nmt")
        {
            variable_nodes = Protocol::Instance()->GetNMTCommandVariables(payload->GetCommandName());
        }
        else
        {
            variable_nodes = Protocol::Instance()->GetCommandVariables(payload->GetCommandName(), payload->GetModuleName());
        }
        
        for (unsigned int n = 0; n < variable_nodes.size(); n++)
        {
            value = payload->GetVariable(variable_nodes[n].GetAttributeValue("name"));
            //LOG.Debug(variable_nodes[n].GetAttributeValue("name") + "=" + value); 
            if (value == "")
            {
                continue;
            }
            
            start_bit = boost::lexical_cast<unsigned int>(variable_nodes[n].GetAttributeValue("start_bit"));
            bit_length = boost::lexical_cast<unsigned int>(variable_nodes[n].GetAttributeValue("bit_length"));
            type = variable_nodes[n].GetAttributeValue("type");
            
            if (highest_bit < start_bit + bit_length - 1)
            {
                highest_bit = start_bit + bit_length - 1;
            }
            
            if (type == "int")
            {
                Protocol::Instance()->EncodeInt(databits, start_bit, bit_length, value);
            }
            else if (type == "float")
            {
                Protocol::Instance()->EncodeFloat(databits, start_bit, bit_length, value);
            }
            else if (type == "ascii")
            {
                Protocol::Instance()->EncodeAscii(databits, start_bit, bit_length, value);
            }
            else if (type == "hexstring")
            {
                Protocol::Instance()->EncodeHexstring(databits, start_bit, bit_length, value);
            }
            else if (type == "enum")
            {
                value = variable_nodes[n].SelectChild("name", value).GetAttributeValue("id");
                Protocol::Instance()->EncodeUint(databits, start_bit, bit_length, value);
            }
            else// if (type == "uint")
            {
                Protocol::Instance()->EncodeUint(databits, start_bit, bit_length, value);
            }
        }
        
        //LOG.Debug(databits.ToDebugString());
        
        unsigned int length = ceil((float)highest_bit / 8.0f);
        
        data[5] = 1;
        data[6] = 0;
        
        data[7] = length;
        
        for (unsigned int n = 0; n < 8; n++)
        {
            data[8 + n] = databits.GetBytes()[n];
        }
        
        data[16] = PACKET_END;
        
        data.SetSize(17);
        
        //LOG.Debug("Bytes: " + data.ToDebugString());
        net::Manager::Instance()->SendTo(this->client_id_, data);
    }
}

void Network::SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, type::Byteset data)
{
    if (client_id != this->client_id_)
    {
            return;
    }
    
    static bool have_start = false;
    
    for (unsigned int n = 0; n < data.GetSize(); n++)
    {
        if (have_start)
        {
            if (data[n] == PACKET_END && this->buffer_.GetSize() == 15)
            {
                this->ProcessBuffer();
                have_start = false;
            }
            else
            {
                this->buffer_.Append(data[n]);
            }
        }
        else if (data[n] == PACKET_START)
        {
            this->buffer_.Clear();
            have_start = true;
        }
        else if (data[n] == PACKET_PING)
        {
            LOG.Info("Received pong.");
        }
    }
}

void Network::SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    if (client_id != this->client_id_)
    {
        return;
    }
    
    if (client_state == net::CLIENT_STATE_DISCONNECTED)
    {
        LOG.Warning("Got disconnected, setting reconnect timer...");
       
        this->timer_id_ = timer::Manager::Instance()->Set(10000, true);
        this->client_id_ = 0;
    }
    else
    {
        LOG.Debug("Got state: " + boost::lexical_cast<std::string>((int)client_state));
    }
}

void Network::SlotOnTimeoutHandler(timer::TimerId timer_id)
{
    try
    {
        this->client_id_ = net::Manager::Instance()->Connect(this->protocol_, this->address_, this->port_or_baud_);
        LOG.Info("Connected again.");
        
        timer::Manager::Instance()->Cancel(timer_id);
        this->timer_id_ = 0;
    }
    catch (std::runtime_error& e)
    {
        LOG.Error(e.what());
        LOG.Warning("Will try again soon...");
    }
}

void Network::ProcessBuffer()
{
    try
    {
        std::string class_name = "";
        std::string direction_name = "";
        std::string module_name = "";
        unsigned int id = 0;
        std::string command_name = "";
        
        unsigned int class_id = (this->buffer_[3] >> 1) & 0x0F;
        //LOG.Debug("class_id=" + boost::lexical_cast<std::string>(class_id));
        class_name = Protocol::Instance()->LookupClassName(class_id);
        
        if (class_name == "nmt")
        {
            unsigned int command_id = this->buffer_[2];
            //LOG.Debug("command_id=" + boost::lexical_cast<std::string>(command_id));
            command_name = Protocol::Instance()->LookupNMTCommandName(command_id);
        }
        else
        {
            unsigned int direction_flag = this->buffer_[3] & 0x01;
            //LOG.Debug("direction_flag=" + boost::lexical_cast<std::string>(direction_flag));
            direction_name = Protocol::Instance()->LookupDirectionFlag(direction_flag);
            
            unsigned int module_id = this->buffer_[2];
            //LOG.Debug("module_id=" + boost::lexical_cast<std::string>(module_id));
            module_name = Protocol::Instance()->LookupModuleName(module_id);
            
            id = this->buffer_[1];
            
            unsigned int command_id = this->buffer_[0];
            //LOG.Debug("command_id=" + boost::lexical_cast<std::string>(command_id));
            command_name = Protocol::Instance()->LookupCommandName(command_id, module_name);
        }

        Message* payload = new Message(class_name, direction_name, module_name, id, command_name);

        unsigned int length = this->buffer_[6];
        
        type::Byteset data_set(length);
        
        for (unsigned int n = 0; n < length; n++)
        {
            data_set.Append(this->buffer_[n + 7]);
        }

        type::Bitset databits(data_set);
        xml::Node::NodeList variable_nodes;
        unsigned int start_bit;
        int bit_length;
        std::string type;
        std::string value;
        std::string name;
        
        if (class_name == "nmt")
        {
            variable_nodes = Protocol::Instance()->GetNMTCommandVariables(command_name);
        }
        else
        {
            variable_nodes = Protocol::Instance()->GetCommandVariables(command_name, module_name);
        }
        
        for (unsigned int n = 0; n < variable_nodes.size(); n++)
        {
            name = variable_nodes[n].GetAttributeValue("name");
            start_bit = boost::lexical_cast<unsigned int>(variable_nodes[n].GetAttributeValue("start_bit"));
            bit_length = boost::lexical_cast<unsigned int>(variable_nodes[n].GetAttributeValue("bit_length"));
            
            if (databits.GetCount() < start_bit + bit_length)
            {
                bit_length = databits.GetCount() - start_bit;
                
                if (bit_length <= 0)
                {
                    LOG.Warning("Can not read variable " + name + " for command " + command_name + ", message is to short, it there a match between the module and the protocol XML file?");
                    //LOG.Debug("start_bit=" + boost::lexical_cast<std::string>(start_bit) + ", bit_length=" + boost::lexical_cast<std::string>(bit_length) + ", databits.GetCount()=" + boost::lexical_cast<std::string>(databits.GetCount()));
                    continue;
                }
            }
            
            type = variable_nodes[n].GetAttributeValue("type");
            
            if (type == "int")
            {
                value = Protocol::Instance()->DecodeInt(databits, start_bit, bit_length);
            }
            else if (type == "float")
            {
                value = Protocol::Instance()->DecodeFloat(databits, start_bit, bit_length);
            }
            else if (type == "ascii")
            {
                value = Protocol::Instance()->DecodeAscii(databits, start_bit, bit_length);
            }
            else if (type == "hexstring")
            {
                value = Protocol::Instance()->DecodeHexstring(databits, start_bit, bit_length);
            }
            else if (type == "enum")
            {
                //LOG.Debug("Enum: command name=" + command_name);
                value = Protocol::Instance()->DecodeUint(databits, start_bit, bit_length);
                //LOG.Debug("Enum: value=" + boost::lexical_cast<std::string>(value));
                //LOG.Debug("start_bit=" + boost::lexical_cast<std::string>(start_bit) + ", bit_length=" + boost::lexical_cast<std::string>(bit_length));
                
                value = variable_nodes[n].SelectChild("id", value).GetAttributeValue("name");
            }
            else// if (type == "uint")
            {
                //LOG.Debug("type: type=" + type);
                value = Protocol::Instance()->DecodeUint(databits, start_bit, bit_length);
            }
            
            payload->SetVariable(name, value);
        }
        
        broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), this)));
    }
    catch (std::runtime_error& e)
    {
        LOG.Error("Malformed message received, " + std::string(e.what()));
        LOG.Debug("Bytes: " + this->buffer_.ToDebugString());
    }
    
    this->buffer_.Clear();
}
    
}; // namespace can
}; // namespace atom
