#include <iostream>

#include <boost/bind.hpp>

#include "net/Manager.h"
#include "net/Types.h"

using namespace atom;

net::Manager::Pointer NM = net::Manager::Instance();

void Main_SlotOnNewState(net::ClientId client_id, net::ClientState client_state)
{
    std::cout << "client:" << client_id << " - New state: " << client_state << std::endl;
}

void Main_SlotOnNewData(net::ClientId client_id, net::Buffer data)
{
    std::string s(data.data());
    
    std::cout << "client:" << client_id << " - New data: " << s << std::endl;
    
    net::Buffer out_data = data;
    
    
    NM->SendTo(client_id, out_data);
}

int main(int argc, char **argv)
{
    std::cout << "connect slots" << std::endl;
    
    NM->ConnectSlots(boost::bind(&Main_SlotOnNewState, _1, _2), boost::bind(&Main_SlotOnNewData, _1, _2));
    
    std::cout << "connect" << std::endl;
    
    try
    {
        net::ServerId server_id = NM->StartServer(net::PROTOCOL_TCP, 2000);
        /*
        net::ClientId client_id_udp = NM->Connect(net::PROTOCOL_UDP, "192.168.1.250", 1100);
        
        std::cout << "connected" << std::endl;
        
        char c[2] = { 251, 0 };
        */
        //NM->SendTo(client_id_udp, std::string(c));
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    std::cout << "sleep" << std::endl;
    
    sleep(100000000);
    
    return 0;
}
