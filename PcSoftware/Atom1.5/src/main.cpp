#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <boost/bind.hpp>
#include <boost/program_options.hpp>

#include "net/Manager.h"
#include "net/types.h"

#include "timer/Manager.h"
#include "timer/types.h"

#include "config/Manager.h"

#include "logging/Logger.h"

using namespace atom;
 

net::Manager::Pointer NM = net::Manager::Instance();

net::ServerId server_id = 0;
timer::TimerId timer_id;

void Main_SlotOnNewState(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
{
    std::cout << "client:" << client_id << " - New state: " << client_state << std::endl;
}

void Main_SlotOnNewData(net::ClientId client_id, net::ServerId server_id, net::Buffer data)
{
    std::string s(data.data());
    
    std::cout << "client:" << client_id << " - New data: " << s << std::endl;
    
    net::Buffer out_data;
    
    strncpy(out_data.c_array(), data.data(), out_data.size());
    
    NM->Disconnect(client_id);
    
    std::cout << "client after disconnect" << s << std::endl;
}

void Main_SlotOnTimeout(timer::TimerId timer_id)
{
    std::cout << "timer:" << timer_id << " - expired" << std::endl;
    
    std::string out_string = "Timeout!";
    
    net::Buffer out_data;
    
    strncpy(out_data.c_array(), out_string.data(), out_data.size());
    
   // NM->SendToAll(server_id, out_data);
}

int main(int argc, char **argv)
{
    logging::Logger LOG("Main");
 
    config::Manager::Pointer CM = config::Manager::Instance();
    
    if (!CM->Set(argc, argv))
    {
        return 0;
    }
    
    if (CM->Exist("MonitorPort"))
    {
        LOG.Info("MonitorPort: " + boost::lexical_cast<std::string>(CM->GetAsInt("MonitorPort")));
    }
    
    if (CM->Exist("CanNet"))
    {
        std::vector<std::string> cannets = CM->GetAsStringVector("CanNet");
        
        for (int n = 0; n < cannets.size(); n++)
        {
            LOG.Info("CanNet[" + boost::lexical_cast<std::string>(n) + "]=" + cannets[n]);
        }
        
    }
   
   /* timer::Manager::Pointer TM = timer::Manager::Instance();
    
    
    TM->Set(10000, true);
    
    TM->ConnectSlots(boost::bind(&Main_SlotOnTimeout, _1));
    */
    
    std::cout << "connect slots2" << std::endl;
    NM->ConnectSlots(boost::bind(&Main_SlotOnNewState, _1, _2, _3), boost::bind(&Main_SlotOnNewData, _1, _2, _3));
    
    
    try
    {
       // server_id = NM->StartServer(net::PROTOCOL_SERIAL, 2000);
    
       // timer_id = TM->Set(10000, true);
       
        
        net::ClientId client_id_udp = NM->Connect(net::PROTOCOL_SERIAL, "/dev/ttyUSB0", 57600);
        
        std::cout << "connected" << std::endl;
        
        char c[2] = { 251, 0 };
        
        std::cout << "before send " << std::endl;
        
        NM->SendTo(client_id_udp, std::string(c));
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    std::cout << "sleep" << std::endl;
    
    sleep(100000000);
    
    return 0;
}
