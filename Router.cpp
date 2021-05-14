/*
 * @Author: your name
 * @Date: 2021-05-13 20:22:13
 * @LastEditTime: 2021-05-14 11:12:32
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Net\Router.cpp
 */
#include "Router.h"

extern RouteTable my_route_table;
extern NextRouters my_next_routers;
extern long local_ip_addr;

DWORD WINAPI Router::Run(LPVOID lpParam)
{
    
}

//timer thread
DWORD WINAPI Router::Timer(LPVOID lpParam)
{
    SOCKET send_socket = *(SOCKET *)lpParam;
    time_t time_init = time(NULL); //initial time: to decide the time to send update message(30s)

    while (1)
    {
        //Timeout detection 
        for (int i = 0; i < my_next_routers.size(); i++)
        {
            time_t time_now = time(NULL);
            time_t time_last = my_next_routers[i].last_update_time;
            if ((time_now - time_last) > EXPIRATION_TIME)
            {
                my_next_routers.erase(i); 
                update_route_table();
            }
            else if ((time_now - time_last) > POSSIBLE_FAILURE_TIME)
            {
                my_next_routers[i].link_cost = INFINITE;
                update_route_table();
            }
        }
        //Send routing control information periodically
        time_t time_now = time(NULL);
        if ((time_now - time_init) % CYCLE == 0)
        {
            for (int i = 0; i < my_route_table.size(); i++)
            {
                Message message;
                message.message_type = Message::ROUTE_CONTROL_MESSAGE;
                message.source_ip_addr = local_ip_addr;
                message.dest_ip_addr = my_route_table[i].dest_ip_addr;
                message.cost = my_route_table[i].cost;
                message.data[0] = '\0';
                broadcast_control_message(send_socket,message);
            }
        }
        Sleep(SECOND);
    }
}
