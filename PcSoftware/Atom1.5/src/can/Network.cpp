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
    
Network::Network(std::string address): Subscriber(false), LOG("can::Network"), buffer_(2048)
{
    this->address_ = address;
    this->client_id_ = 0;
       
    net::Manager::Instance()->ConnectSlots(net::Client::SignalOnNewState::slot_type(&Network::SlotOnNewState, this, _1, _2, _3).track(this->tracker_),
                                           net::Client::SignalOnNewData::slot_type(&Network::SlotOnNewData, this, _1, _2, _3).track(this->tracker_));
    
    // Examples of address
    // udp:192.168.1.250:1100
    // serial:/dev/ttyUSB0:38400
    
    std::vector<std::string> parts;
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
        
        LOG.Info("Sending ping.");
        type::Byteset buffer(1);
        
        buffer[0] = PACKET_PING;
        
        net::Manager::Instance()->SendTo(this->client_id_, buffer);
    }
    catch (std::exception e)
    {
        LOG.Error(e.what());
    }
}

Network::~Network()
{
    net::Manager::Instance()->Disconnect(this->client_id_);
}

void Network::SlotOnNewData(net::ClientId client_id, net::ServerId server_id, type::Byteset data)
{
    if (client_id == this->client_id_)
    {
        type::Byteset temp_buffer = data;
        this->io_service_.post(boost::bind(&Network::SlotOnNewDataHandler, this, client_id, server_id, temp_buffer));
    }
}

void Network::SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    if (client_id == this->client_id_)
    {
        this->io_service_.post(boost::bind(&Network::SlotOnNewStateHandler, this, client_id, server_id, client_state));
    }
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
        
        
        
    }
}

void Network::SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, type::Byteset data)
{
    static bool have_start = false;
    
    for (unsigned int n = 0; n < data.GetSize(); n++)
    {
        if (have_start)
        {
            if (data[n] == PACKET_END && this->buffer_.GetSize() == 15)
            {
                //LOG.Debug("Received packet end and size is 15.");
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
            //LOG.Debug("Received packet start.");
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
    catch (std::exception e)
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
        
        //LOG.Debug("class_id=" + boost::lexical_cast<std::string>(class_id) + ", byte[3] = " + boost::lexical_cast<std::string>((unsigned int)this->buffer_[3]));
        
        class_name = Protocol::Instance()->LookupClassName(class_id);
        
        //LOG.Debug("class_name=" + class_name);
        
        if (class_name == "nmt")
        {
            unsigned int command_id = this->buffer_[2];
            //LOG.Debug("command_id=" + boost::lexical_cast<std::string>(command_id) + ", byte[2] = " + boost::lexical_cast<std::string>((unsigned int)this->buffer_[2]));
            
            command_name = Protocol::Instance()->LookupNMTCommandName(command_id);
            //LOG.Debug("command_name=" + command_name);
        }
        else
        {
            unsigned int direction_flag = this->buffer_[3] & 0x01;
            //LOG.Debug("direction_flag=" + boost::lexical_cast<std::string>(direction_flag) + ", byte[3] = " + boost::lexical_cast<std::string>((unsigned int)this->buffer_[3]));
            
            direction_name = Protocol::Instance()->LookupDirectionFlag(direction_flag);
            //LOG.Debug("direction_name=" + direction_name);
            
            unsigned int module_id = this->buffer_[2];
            //LOG.Debug("module_id=" + boost::lexical_cast<std::string>(module_id) + ", byte[2] = " + boost::lexical_cast<std::string>((unsigned int)this->buffer_[2]));
            
            module_name = Protocol::Instance()->LookupModuleName(module_id);
            //LOG.Debug("module_name=" + module_name);
            
            id = this->buffer_[1];
            //LOG.Debug("id=" + boost::lexical_cast<std::string>(id) + ", byte[1] = " + boost::lexical_cast<std::string>((unsigned int)this->buffer_[1]));
            
            unsigned int command_id = this->buffer_[0];
            //LOG.Debug("command_id=" + boost::lexical_cast<std::string>(command_id) + ", byte[0] = " + boost::lexical_cast<std::string>((unsigned int)this->buffer_[0]));
            
            command_name = Protocol::Instance()->LookupCommandName(command_id, module_name);
            //LOG.Debug("command_name=" + command_name);
        }

        Message* payload = new Message(class_name, direction_name, module_name, id, command_name);

        unsigned int length = this->buffer_[6];
        //LOG.Debug("Data length = " + boost::lexical_cast<std::string>(length));
        
        type::Byteset data_set(length);
        
        for (unsigned int n = 0; n < length; n++)
        {
            //LOG.Debug("add byte[" + boost::lexical_cast<std::string>(n + 7) + "] = " + boost::lexical_cast<std::string>((unsigned int)this->buffer_[n + 7]));
            data_set.Append(this->buffer_[n + 7]);
        }
        /*
        for (unsigned int n = 0; n < data_set.GetSize(); n++)
        {
            LOG.Debug("byte[" + boost::lexical_cast<std::string>(n) + "] = " + boost::lexical_cast<std::string>((unsigned int)data_set[n]));
        }
        */
        type::Bitset databits(data_set);
        
        /*
        std::string temp = "";
        std::string temp2 = "";
        for (unsigned int n = 0; n < databits.GetCount(); n ++)
        {
            
            temp2 += boost::lexical_cast<std::string>((unsigned int)databits.bytes_[n/8]);
            temp += boost::lexical_cast<std::string>(databits.Get(n));
        }
        
        LOG.Debug("databits1 = " + temp);
        LOG.Debug("databits2 = " + temp2);*/
        
        xml::Node::NodeList variable_nodes;
        
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
            unsigned int start_bit = boost::lexical_cast<unsigned int>(variable_nodes[n].GetAttributeValue("start_bit"));
            unsigned int bit_length = boost::lexical_cast<unsigned int>(variable_nodes[n].GetAttributeValue("bit_length"));
            std::string type = variable_nodes[n].GetAttributeValue("type");
            
            //LOG.Debug("name=" + variable_nodes[n].GetAttributeValue("name"));
            //LOG.Debug("type=" + type);
            
            std::string value = "";
            
            if (type == "int")
            {
                unsigned long raw_bit_value = databits.Read(start_bit, bit_length);
                int raw_value = 0;
                
                memcpy(&raw_value, &raw_bit_value, sizeof(raw_value));
                value = boost::lexical_cast<std::string>(raw_value);
            }
            else if (type == "float")
            {
                float raw_value = 0;
                bool negative = false;;
                
                if (databits.Get(start_bit))
                {
                    negative = true;
                }
                
                for (int c = bit_length-1; c > 0; c--)
                {
                    if (databits.Get(start_bit + c) != negative)
                    {
                        raw_value += pow(2.0f, (int)(bit_length-1-c));
                    }
                }
                
                raw_value /= 64.0f;
                
                if (negative)
                {
                    raw_value = -raw_value;
                }
                
                value = boost::lexical_cast<std::string>(raw_value);
                //LOG.Debug("float:value="+value);
            }
            else if (type == "ascii")
            {
                char c = 0;
                
                for (unsigned int p = 0; p < bit_length; p += 8)
                {
                    unsigned long raw_bit_value = databits.Read(start_bit + p, 8);
                    memcpy(&c, &raw_bit_value, sizeof(c));
                    value += c;
                }
            }
            else if (type == "hexstring")
            {
                for (unsigned int p = 0; p < bit_length; p += 4)
                {
                    unsigned long raw_bit_value = databits.Read(start_bit + p, 4);
                    
                    //value += boost::lexical_cast<std::string>(raw_bit_value);
                }
            }
            else if (type == "enum")
            {
                value = boost::lexical_cast<std::string>(databits.Read(start_bit, bit_length));
                value = variable_nodes[n].SelectChild("id", value).GetAttributeValue("name");
            }
            else// if (type == "uint")
            {
                unsigned long raw_bit_value = databits.Read(start_bit, bit_length);
                unsigned int raw_value = 0;
                
                memcpy(&raw_value, &raw_bit_value, sizeof(raw_value));
                value = boost::lexical_cast<std::string>(raw_value);
            }
            
            //LOG.Debug("start_bit=" + boost::lexical_cast<std::string>(start_bit) + ",bit_length=" + boost::lexical_cast<std::string>(bit_length));
            
            payload->SetVariable(variable_nodes[n].GetAttributeValue("name"), value);
            //LOG.Debug("Variable:" + variable_nodes[n].GetAttributeValue("name") + " = " + value);
        }
        
        broker::Manager::Instance()->Post(broker::Message::Pointer(new broker::Message(broker::Message::CAN_MESSAGE, broker::Message::PayloadPointer(payload), this)));
    }
    catch (std::runtime_error& e)
    {
        std::string bytestring;
        
        for (unsigned int n = 0; n < this->buffer_.GetSize(); n++)
        {
            bytestring += boost::lexical_cast<std::string>((unsigned int)this->buffer_[n]) + ",";
        }
        
        LOG.Debug("Bytes: " + bytestring);
        
        LOG.Error("Malformed message received, " + std::string(e.what()));
    }
    
    this->buffer_.Clear();
}
    
}; // namespace can
}; // namespace atom
