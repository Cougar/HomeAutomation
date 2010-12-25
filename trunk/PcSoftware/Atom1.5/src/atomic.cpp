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

#include <string>
#include <vector>
#include <iostream>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>

#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/locks.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include "config.h"

#include "net/Manager.h"
#include "net/Subscriber.h"
#include "net/types.h"

#include "common/common.h"

#include <readline/readline.h>
#include <readline/history.h>

using namespace atom;

bool finish = false;
common::StringList autocomplete_list;
char* buffer = NULL;
boost::condition on_message_condition;
struct termios original_flags;
std::string history_filename;

void Handler(int status);
void CleanUp();

char* AutoCompleteGet(const char* text, int state);
static char** AutoComplete(const char* text, int start, int end);

std::string GetUserHomeDirectory()
{
    return std::string(getpwuid(getuid())->pw_dir);
}

class ConsoleClient : public net::Subscriber
{
public:
    typedef boost::shared_ptr<ConsoleClient> Pointer;
    
    ConsoleClient(std::string address, unsigned int port)
    {
        this->client_id_ = net::Manager::Instance()->Connect(net::PROTOCOL_TCP, address, port);
    }
    
    virtual ~ConsoleClient()
    {
        net::Manager::Instance()->Disconnect(this->client_id_);

        this->io_service_.stop();
    }
    
    std::string GetPrompt()
    {
        return this->prompt_;
    }
    
    void SendResponse(std::string payload)
    {
        std::string packet = "RESP";
        packet += common::PadNumber(payload.length() + 1, 4);
        packet += payload;
        
        net::Manager::Instance()->SendTo(this->client_id_, packet);
    }
    
    void AutoCompleteRequest(unsigned int arg_index, std::string commandline)
    {
        std::string payload = common::PadNumber(arg_index, 4);
        payload += commandline;
        
        std::string packet = "COMP";
        packet += common::PadNumber(payload.length() + 1, 4);
        packet += payload;
        
        net::Manager::Instance()->SendTo(this->client_id_, packet);
    }
    
private:
    net::ClientId client_id_;
    std::string prompt_;
    
    
    void SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
    {
        if (client_state != net::CLIENT_STATE_CONNECTED)
        {
            std::cout << "Disconnected from server" << std::endl;
            
            this->client_id_ = 0;
            finish = true;
           
            kill(getpid(), SIGTERM);
        }
    }
    
    void SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, common::Byteset data)
    {
        std::string s(data.ToCharString());
 
        while (s.length() > 0)
        {
            std::string command = s.substr(0, 4);
            unsigned int payload_length = boost::lexical_cast<unsigned int>(s.substr(4, 4));
            
            if (command == "TEXT")
            {
                std::cout << s.substr(8, payload_length - 1) << std::flush;
            }
            else if (command == "PROM")
            {
                this->prompt_ = s.substr(8, payload_length - 1);
                on_message_condition.notify_all();
            }
            else if (command == "COMP")
            {
                autocomplete_list.clear();
                
                if (payload_length > 0)
                {
                    std::string payload = s.substr(8, payload_length - 1);
                    
                    if (payload.length() > 0)
                    {
                        boost::algorithm::split(autocomplete_list, payload, boost::is_any_of("\n"), boost::algorithm::token_compress_on);
                    }
                }
                
                on_message_condition.notify_all();
            }
            else
            {
                std::cerr << "Unknown data received: " << s << std::endl;
                finish = true;
                on_message_condition.notify_all();
                break;
            }
            
            if (8 + payload_length >= s.length())
            {
                break;
            }
            
            s = s.substr(8 + payload_length);
        }
    }
};

ConsoleClient::Pointer cc;


int main(int argc, char **argv)
{
    // Signal handlers
    signal(SIGTERM, Handler);
    signal(SIGINT, Handler);
    signal(SIGQUIT, Handler);
    signal(SIGABRT, Handler);
    signal(SIGPIPE, Handler);
    
    boost::mutex guard_mutex;
    
    // Setup readline
    history_filename = GetUserHomeDirectory() + "/.atomic_history";
    read_history(history_filename.data());
    
    rl_attempted_completion_function = AutoComplete;
    
    // Save command line state
    tcgetattr(fileno(stdin), &original_flags);
    
    // Parse commandline
    boost::program_options::options_description command_line;
    boost::program_options::variables_map variable_map;
    
    command_line.add_options()
    ("help,h",    "produce help message")
    ("server,s",  boost::program_options::value<std::string>()->default_value("localhost"), "server address")
    ("port,p",    boost::program_options::value<unsigned int>()->default_value(1202), "server port");
    
    try
    {
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(command_line).run(), variable_map);
    }
    catch (boost::program_options::unknown_option e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << command_line << std::endl;
        CleanUp();
        return EXIT_FAILURE;
    }
    catch (boost::program_options::invalid_syntax e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << command_line << std::endl;
        CleanUp();
        return EXIT_FAILURE;
    }
    
    if (variable_map.count("help") != 0)
    {
        std::cout << command_line << std::endl;
        CleanUp();
        return EXIT_SUCCESS;
    }
    
    
    std::cout << "Atom Interactive Console, version " + std::string(VERSION) + " starting..." << std::endl;
    std::cout << "Written by Mattias Runge 2010." << std::endl;
    std::cout << "Released under GPL version 2." << std::endl;
    
    net::Manager::Create();
       
    std::cout << "Connecting to " << variable_map["server"].as<std::string>().data() << ":" << variable_map["port"].as<unsigned int>() << "..." << std::endl;
    
    try
    {
        cc = ConsoleClient::Pointer(new ConsoleClient(variable_map["server"].as<std::string>(), variable_map["port"].as<unsigned int>()));
    }
    catch (std::runtime_error& e)
    {
        std::cout << "error!" << std::endl;
        std::cerr << e.what() << std::endl;
        CleanUp();
        return EXIT_FAILURE;
    }
    
    while (true)
    {
        boost::mutex::scoped_lock guard(guard_mutex);
        on_message_condition.wait(guard);
        
        if (finish)
        {
            break;
        }
        
        while ((buffer = readline(cc->GetPrompt().data())) != NULL)
        {
            if (strlen(buffer) == 0)
            {
                continue;
            }
            
            break;
        }
        
        cc->SendResponse(buffer);
        add_history(buffer);
    }
    
    CleanUp();
    
    return EXIT_SUCCESS;
}

static char** AutoComplete(const char* text, int start, int end)
{
    unsigned int count = 0;
    boost::mutex guard_mutex;

    for (unsigned int n = 0; n < start; n++)
    {
        if (rl_line_buffer[n] == ' ')
        {
            count++;
        }
    }
    
    cc->AutoCompleteRequest(count, rl_line_buffer);
    
    boost::mutex::scoped_lock guard(guard_mutex);
    on_message_condition.wait(guard);
    
    /*if (autocomplete_list.size() == 0)
    {
        return NULL;
    }*/
    
    return rl_completion_matches(text, &AutoCompleteGet);
}

char* AutoCompleteGet(const char* text, int state)
{
    static int index = 0;
    int length = strlen(text);
    std::string name;
    
    if (!state) // First run
    {
        index = 0;
    }
    
    while (autocomplete_list.size() > index)
    {
        name = autocomplete_list[index];
        
        index++;
        
        if (strncmp(name.data(), text, length) == 0)
        {
            char *result = (char*)malloc(name.size() + 1);
            strcpy(result, name.data());
            
            return result;
        }
    }
    
    return NULL;
}

void CleanUp()
{
    std::cout << "Cleaning up..." << std::endl;
 
    write_history(history_filename.data());
    
    cc.reset();
    
    net::Manager::Delete();
    
    if (buffer != NULL)
    {
        free(buffer);
    }
    
    std::cout << "Thank you for using Atom. Goodbye!" << std::endl;
    
    tcsetattr(fileno(stdin), TCSANOW, &original_flags); // Restore
}

void Handler(int status)
{
    std::string signal_name = "Unknown";
    
    switch (status)
    {
        case SIGTERM:
        {
            signal_name = "Terminate";
            break;
        }   
        case SIGINT:
        {
            signal_name = "Interupt";
            break;
        }   
        case SIGQUIT:
        {
            signal_name = "Quit";
            break;
        }   
        case SIGABRT:
        {
            signal_name = "Abort";
            break;
        }   
        case SIGIO:
        {
            signal_name = "I/O";
            break;
        }   
        case SIGPIPE:
        {
            signal_name = "Pipe";
            break;
        }
    }
    
    
    if (status != SIGPIPE)
    {
        CleanUp();
        exit(0);
    }
}