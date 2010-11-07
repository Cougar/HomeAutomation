#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <boost/bind.hpp>
#include <boost/program_options.hpp>

#include "net/Manager.h"
#include "net/Types.h"

#include "timer/Manager.h"
#include "timer/Types.h"

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
    
    NM->SendTo(client_id, out_data);
}

void Main_SlotOnTimeout(timer::TimerId timer_id)
{
    std::cout << "timer:" << timer_id << " - expired" << std::endl;
    
    std::string out_string = "Timeout!";
    
    net::Buffer out_data;
    
    strncpy(out_data.c_array(), out_string.data(), out_data.size());
    
    NM->SendToAll(server_id, out_data);
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
    LOG.Info("Done!");
    
    return 0;
    
    // Declare a group of options that will be
    // allowed only on command line
    boost::program_options::options_description generic("Generic options");
    generic.add_options()   ("version,v", "print version string")
    ("help", "produce help message");
    
    // Declare a group of options that will be
    // allowed both on command line and in
    // config file
    boost::program_options::options_description config("Configuration");
    config.add_options()
                        ("MonitorPort", boost::program_options::value<int>()->default_value(2000), "TCP port to expose for monitoring")
                        ("CommandPort", boost::program_options::value<int>()->default_value(2001), "TCP port to expose for monitoring")
                        ("LogFile", boost::program_options::value<std::string>(), "File to log output to")
                        ("ScriptsPath", boost::program_options::value<std::string>(), "File to log output to")
                        ("ProtocolFile", boost::program_options::value<std::string>(), "File to log output to")
                        ("DaemonMode,D", "File to log output to")
                        ("CanNet_1", boost::program_options::value<std::vector<std::string> >(), "include path");
    
   LOG.Debug("a");
    boost::program_options::options_description cmdline_options;
    cmdline_options.add(generic).add(config);
    
    boost::program_options::options_description config_file_options;
    config_file_options.add(config);
    
    boost::program_options::options_description visible("Allowed options");
    visible.add(generic).add(config);
    
        
    boost::program_options::variables_map vm;
    store(boost::program_options::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
    LOG.Debug("b"); 
    std::ifstream ifs("atom.conf");
    store(parse_config_file(ifs, config_file_options), vm);
    notify(vm);
    
    LOG.Debug("c");
    
    if (vm.count("help"))
    {
        LOG.Info("Help");
    }
    
    if (vm.count("MonitorPort"))
    {
        LOG.Info("MonitorPort: " + boost::lexical_cast<std::string>(vm["MonitorPort"].as<int>()));
    }
    
    if (vm.count("LogFile"))
    {
        LOG.Info("LogFile: " + vm["LogFile"].as<std::string>());
    }
    
    if (vm.count("DaemonMode"))
    {
        LOG.Info("DaemonMode: " + vm["DaemonMode"].as<std::string>());
    }
  
    
    
    
    
    
    
    
    
    
    
    net::Manager::Delete();
    
    
    
    
    return 0;
    
    LOG.Error("test sdsdsds sds ");
    return 0;
    timer::Manager::Pointer TM = timer::Manager::Instance();
    
    
    //TM->Set(10000, true);
    
    TM->ConnectSlots(boost::bind(&Main_SlotOnTimeout, _1));
    
    
    std::cout << "connect slots2" << std::endl;
    NM->ConnectSlots(boost::bind(&Main_SlotOnNewState, _1, _2, _3), boost::bind(&Main_SlotOnNewData, _1, _2, _3));
    
    
    try
    {
        server_id = NM->StartServer(net::PROTOCOL_TCP, 2000);
    
        timer_id = TM->Set(10000, true);
       
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
