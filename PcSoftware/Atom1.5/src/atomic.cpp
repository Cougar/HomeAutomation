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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>

#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/locks.hpp>
#include <boost/algorithm/string.hpp>

#include "net/Manager.h"
#include "net/Subscriber.h"
#include "net/types.h"

#include "common/common.h"

#include <readline/readline.h>
#include <readline/history.h>

using namespace atom;


void Handler(int status);
void CleanUp();

char* AutoCompleteGet(const char* text, int state);
static char** AutoComplete(const char* text, int start, int end);

common::StringList autocomplete_list;
char* buffer = NULL;
bool waiting_for_autocomplete = false;
boost::condition on_message_condition;
boost::mutex guard_mutex;
std::string prompt;
int prompt_id = -1;

class ConsoleClient : public net::Subscriber
{
public:
    typedef boost::shared_ptr<ConsoleClient> Pointer;
    
    ConsoleClient(std::string address, unsigned int port)
    {
        this->identifier_ = address + ":" + boost::lexical_cast<std::string>(port);
        this->client_id_ = net::Manager::Instance()->Connect(net::PROTOCOL_TCP, address, port);
    }
    
    virtual ~ConsoleClient()
    {
        net::Manager::Instance()->Disconnect(this->client_id_);

        this->io_service_.stop();
    }
    
    void Send(std::string data)
    {
        net::Manager::Instance()->SendTo(this->client_id_, data);
    }
    
    bool IsConnected()
    {
        return this->client_id_ != 0;
    }
    
    std::string GetIdentifier()
    {
        return this->identifier_;
    }
    
private:
    net::ClientId client_id_;
    std::string identifier_;
    
    
    void SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state)
    {
        if (client_state == net::CLIENT_STATE_CONNECTED)
        {
            printf("Connected to Atom Daemon!\n");
        }
        else
        {
            printf("\nDisconnected!\n");
            this->client_id_ = 0;
           
            kill(getpid(), SIGTERM);
        }
    }
    
    void SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, common::Byteset data)
    {
        prompt_id = -1;
        
        std::string s(data.ToCharString());
        
        if (waiting_for_autocomplete)
        {
            autocomplete_list.clear();
            
            boost::algorithm::trim_if(s, boost::is_any_of("\n"));
            boost::algorithm::trim_if(s, boost::is_any_of(" "));
            
            if (s != "")
            {
                boost::algorithm::split(autocomplete_list, s, boost::is_any_of("\n"), boost::algorithm::token_compress_on);
            }
        }
        else
        {
            common::StringList lines;
            
            boost::algorithm::split(lines, s, boost::is_any_of("\n"), boost::algorithm::token_compress_on);
            
            for (unsigned int n = 0; n < lines.size(); n++)
            {
                common::StringList parts;
                
                boost::algorithm::split(parts, lines[n], boost::is_any_of(";"), boost::algorithm::token_compress_off);
                
                if (parts[0].length() == 1 && parts[0] == "P")
                {
                    prompt_id = boost::lexical_cast<unsigned int>(parts[1]);
                    
                    prompt = "";
                    
                    for (unsigned int c = 2; c < parts.size(); c++)
                    {
                        prompt += parts[c] + " ";
                    }
                }
                else 
                {
                    boost::algorithm::trim_if(lines[n], boost::is_any_of(" "));
                    
                    if (lines[n] != "")
                    {
                        printf("%s\n", lines[n].data());
                    }
                }
            }
        }
         
        on_message_condition.notify_all();
    }
};

ConsoleClient::Pointer cc;
struct termios original_flags;
std::string history_filename;

int main(int argc, char **argv)
{
    signal(SIGTERM, Handler);
    signal(SIGINT, Handler);
    signal(SIGQUIT, Handler);
    signal(SIGABRT, Handler);
    signal(SIGPIPE, Handler);
    
    tcgetattr(fileno(stdin), &original_flags);
       
    printf("Atom Interactive Console, version 1.5.0 starting...\n");
    printf("Written by Mattias Runge 2010.\n");
    printf("Released under GPL version 2.\n");
    
    net::Manager::Create();
    
    std::string address = "localhost";
    unsigned int port = 1202;
    
    printf("Connecting to %s:%d...", address.data(), port);
    
    try
    {
        cc = ConsoleClient::Pointer(new ConsoleClient(address, port));
    }
    catch (std::runtime_error& e)
    {
        printf("%s\n", e.what());
        CleanUp();
        
        return EXIT_FAILURE;
    }
    
    printf("success!\n");
    
    prompt = cc->GetIdentifier() + "] ";
    
    passwd* user_struct = getpwuid(getuid());
    
    history_filename = std::string(user_struct->pw_dir) + "/.atomic_history";
    
    read_history(history_filename.data());
    
    rl_attempted_completion_function = AutoComplete;
    
    while ((buffer = readline(prompt.data())) != NULL)
    {
        if (strlen(buffer) == 0)
        {
            continue;
        }
        
        if (strcmp(buffer, "quit") == 0)
        {
            break;
        }
        
        waiting_for_autocomplete = false;
        prompt = cc->GetIdentifier() + "] ";
        
        if (prompt_id == -1)
        {
            cc->Send("E;" + std::string(buffer));
            add_history(buffer);
        }
        else
        {
            cc->Send("R;" + boost::lexical_cast<std::string>(prompt_id) + ";" + std::string(buffer));
            prompt_id = -1;
        }
        
        boost::mutex::scoped_lock guard(guard_mutex);
        on_message_condition.wait(guard);    
    }
    
    CleanUp();
    
    return EXIT_SUCCESS;
}

static char** AutoComplete(const char* text, int start, int end)
{
    unsigned int count = 0;
    
    //printf("\nAutoComplete:start=%d, end=%d\n", start, end);
    
    for (unsigned int n = 0; n < start; n++)
    {
        if (rl_line_buffer[n] == ' ')
        {
            count++;
        }
    }
    
    waiting_for_autocomplete = true;
   

    std::string request = "A;"+ boost::lexical_cast<std::string>(count) + ";" + std::string(rl_line_buffer);
    //printf("\nSending request:%s\n", request.data());
    
    cc->Send(request);
    
    boost::mutex::scoped_lock guard(guard_mutex);
    on_message_condition.wait(guard);    
    
    if (autocomplete_list.size() == 0)
    {
        return NULL;
    }
    
    //printf("processing response\n");
    
    return rl_completion_matches(text, &AutoCompleteGet);
}

char* AutoCompleteGet(const char* text, int state)
{
    static int index = 0;
    int length = strlen(text);
    std::string name;
    
    //printf("run, %s, %d, %d\n", text, state, autocomplete_list.size());
    
    if (!state) // First run
    {
        index = 0;
    }
    
    while (autocomplete_list.size() > index)
    {
        name = autocomplete_list[index];
        //printf("testing name = %s\n", name.data());
        
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
    printf("Cleaning up...\n");
 
    write_history(history_filename.data());
    
    cc.reset();
    
    net::Manager::Delete();
    
    if (buffer != NULL)
    {
        free(buffer);
    }
    
    printf("Thank you for using Atom. Goodbye!\n");
    
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
        //on_message_condition.notify_all();
    }
}