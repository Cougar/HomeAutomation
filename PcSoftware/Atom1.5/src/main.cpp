#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <stdio.h>

#include <boost/bind.hpp>
#include <boost/program_options.hpp>

#include "net/Manager.h"
#include "net/types.h"

#include "timer/Manager.h"
#include "timer/types.h"

#include "config/Manager.h"

#include "logging/Logger.h"

#include "can/Network.h"
#include "can/Monitor.h"
#include "can/Protocol.h"

using namespace atom;
 
logging::Logger LOG("Main");

net::Manager::Pointer NM = net::Manager::Instance();

net::ServerId server_id = 0;
timer::TimerId timer_id;

void Main_SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    std::cout << "client:" << client_id << " - New state: " << client_state << std::endl;
}

void Main_SlotOnNewData(net::ClientId client_id, net::ServerId server_id, type::Byteset data)
{
    char* t = new char[2048];
    
    memset(t, 0, sizeof(t));
    
    //LOG.Info("size:" + boost::lexical_cast<std::string>(data.GetSize()*3+2));
    
    for (unsigned int n = 0; n < data.GetSize(); n++)
    {
        unsigned char c = data[n];
    
        //LOG.Info(boost::lexical_cast<std::string>(n) + ":" + boost::lexical_cast<std::string>((unsigned int)c));
        
        //std::cout << std::dec << n << ":(dec)" << std::dec << (unsigned int)c << std::endl;
        std::cout << std::dec << n << ":(hex)" << std::hex << (unsigned int)c << std::endl;
        
        sprintf(t + n*2, "%02X", c);
    }
    
    
    std::cout << std::dec << "client:" << client_id << " - New data: " << t << std::endl;
    
    delete [] t;
}

void Main_SlotOnTimeout(timer::TimerId timer_id)
{
 /*   std::cout << "timer:" << timer_id << " - expired" << std::endl;
    
    std::string out_string = "Timeout!";
    
    type::Byteset out_data;
    
    strncpy(out_data.GetRaw(), out_string.data(), out_data.size());
    
   // NM->SendToAll(server_id, out_data);*/
}

int main(int argc, char **argv)
{
    std::vector<broker::Subscriber::Pointer> subscribers;
 
    config::Manager::Pointer CM = config::Manager::Instance();
    
    if (!CM->Set(argc, argv))
    {
        NM->Delete();
        return 0;
    }
    
    if (CM->Exist("ProtocolFile"))
    {
        LOG.Info("ProtocolFile: " + CM->GetAsString("ProtocolFile"));
        
        can::Protocol::Instance()->Load(CM->GetAsString("ProtocolFile"));
    }
    
    if (CM->Exist("MonitorPort"))
    {
        LOG.Info("MonitorPort: " + boost::lexical_cast<std::string>(CM->GetAsInt("MonitorPort")));
        
        subscribers.push_back(can::Monitor::Pointer(new can::Monitor(CM->GetAsInt("MonitorPort"))));  
    }
    
    if (CM->Exist("CanNet"))
    {
        type::StringList cannets = CM->GetAsStringVector("CanNet");
        
        for (int n = 0; n < cannets.size(); n++)
        {
            LOG.Info("CanNet[" + boost::lexical_cast<std::string>(n) + "]=" + cannets[n]);
            
            subscribers.push_back(can::Network::Pointer(new can::Network(cannets[n])));            
        }
        
    }
    
    
   
   /* timer::Manager::Pointer TM = timer::Manager::Instance();
    
    
    TM->Set(10000, true);
    
    TM->ConnectSlots(boost::bind(&Main_SlotOnTimeout, _1));
    */
    
   // std::cout << "connect slots2" << std::endl;
   // NM->ConnectSlots(boost::bind(&Main_SlotOnNewState, _1, _2, _3), boost::bind(&Main_SlotOnNewData, _1, _2, _3));
    
    
    try
    {
       // server_id = NM->StartServer(net::PROTOCOL_SERIAL, 2000);
    
       // timer_id = TM->Set(10000, true);
       
        
        //net::ClientId client_id_udp = NM->Connect(net::PROTOCOL_UDP, "192.168.1.250", 1100);
        //net::ClientId client_id_udp = NM->Connect(net::PROTOCOL_SERIAL, "/dev/ttyUSB0", 57600);
        
        //std::cout << "connected" << std::endl;
        
        /*char c[2] = { 251, 0 };
        
        std::cout << "before send " << std::endl;
        
        NM->SendTo(client_id_udp, c);*/
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    std::cout << "sleep" << std::endl;
    
    sleep(100000000);
    
    return 0;
}
