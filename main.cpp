#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <cstring>
#include <windows.h>
#include <cmath>
#include "Router.h"
#define HEADER "Router-> "
#define LEN 20
using namespace std;

/**
 * @description: run the router
 * @param {Router*} lpParam
 * @return {*}
 */
DWORD WINAPI run(LPVOID lpParam)
{
    Router *router_ptr = (Router *)lpParam;
    router_ptr->run();
}

/**
 * @description: run the timer
 * @param {Rouer*} lpParam
 * @return {*}
 */
DWORD WINAPI timer(LPVOID lpParam)
{
    Router *router_ptr = (Router *)lpParam;
    router_ptr->timer();
}

int main()
{
    Router *router = new Router();
    router->initial();

    //Input the initial my_next_routers information
    cout << "Initialization:" << endl;
    //input ip
    string str_local_ip_addr;
    cout << "Local IP address: ";
    cin >> str_local_ip_addr;
    router->local_ip_addr = inet_addr(str_local_ip_addr.c_str());
    //input algorithm
    cout << "Routing Algorithm(DV/LS): ";
    string str_algorithm;
    cin >> str_algorithm;
    if (str_algorithm == "DV")
        router->algorithm = Router::DV;
    else if (str_algorithm == "LS")
        router->algorithm = Router::LS;
    else
    {
        cout << "Invalid Type" << endl;
        return 0;
    }
    //cmd
    string command;
    while (1)
    {
        cout << HEADER;
        cin >> command;
        if (command == "ADD" || command == "add")
        {
            string str_ip_addr;
            u_short port;
            int cost;
            cin >> str_ip_addr >> port >> cost;
            router->add(inet_addr(str_ip_addr.c_str()), port, cost);
        }
        else if (command == "Run" || command == "run")
        {
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)run, (void *)router, 0, NULL);
            //CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)timer,(void*)router, 0, NULL);
        }
        else if (command == "show")
        {
            string str;
            getline(cin, str);
            if (str == " route table")
            {
                router->show_route_table();
            }
            else if (str == " next router")
            {
                router->show_next_routers();
            }
        }
        else if (command == "send")
        {
            Message message;
            string str;
            cin >> str;
            message.message_type = Message::DATA_MESSAGE;
            message.source_ip_addr = router->local_ip_addr;
            message.dest_ip_addr = inet_addr(str.c_str());
            message.cost = 0;
            cin >> str;
            memset(message.data,0,sizeof(message.data));
            strcpy(message.data, str.c_str());
            router->send_data_message(message);
        }
        else if (command == "exit")
        {
            break;
        }
    }
}
