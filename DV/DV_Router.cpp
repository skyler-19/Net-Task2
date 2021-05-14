/*
 * @Author: your name
 * @Date: 2021-05-13 10:11:04
 * @LastEditTime: 2021-05-14 17:11:42
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Net\DV\Router.cpp
 */

#include "Router.h"


DWORD WINAPI Router::Run(LPVOID lpParam)
{
    WSA_initialization();
    SOCKET recv_socket = create_recv_socket();
    SOCKET send_socket = create_send_socket();
    broadcast(send_socket);
    while(1)
    {
        
    }
}

bool Router::update_net_state(Message message)
{
    return my_next_routers.update(message);
}

//traverse all distance vectors of all neighboring routers,
//if it's cost less than current value or the dest ip hasn't been included,
//update the route table
bool Router::update_route_table()
{
    bool is_changed = false;
    for (int i = 0; i < my_next_routers.size(); i++)
    {
        NextRouters::NextRouter &curr_router = my_next_routers[i];
        NextRouters::DistanceVectorTable &curr_dis_vec_table = curr_router.distance_vecor_table;
        for (int j = 0; j < curr_dis_vec_table.size(); j++)
        {
            int curr_distance = curr_dis_vec_table[j].distance;
            long curr_dest_ip = curr_dis_vec_table[j].ip_addr;
            bool is_find = false;
            for (int t = 0; t < my_route_table.size(); t++)
            {
                if (curr_dest_ip == my_route_table[t].dest_ip_addr)
                {
                    is_find = true;
                    if (curr_router.link_cost + curr_distance < my_route_table[t].cost)
                    {
                        is_changed = true;
                        my_route_table[t].next_hop_ip_addr = curr_router.ip_addr;
                        my_route_table[t].cost = curr_router.link_cost + curr_distance;
                    }
                    break;
                }
            }
            if (!is_find)
            {
                is_changed = true;
                my_route_table.push(curr_dest_ip, curr_router.ip_addr, curr_router.link_cost + curr_distance);
            }
        }
    }
    return is_changed;
}

void Router::broadcast(SOCKET send_socket)
{
    for (int i = 0; i < my_route_table.size(); i++)
    {
        Message message;
        message.message_type = Message::ROUTE_CONTROL_MESSAGE;
        message.source_ip_addr = local_ip_addr;
        message.dest_ip_addr = my_route_table[i].dest_ip_addr;
        message.cost = my_route_table[i].cost;
        message.data[0] = '\0';
        broadcast_control_message(send_socket, message);
    }
}

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
                broadcast(send_socket);
            }
            else if ((time_now - time_last) > POSSIBLE_FAILURE_TIME)
            {
                my_next_routers[i].link_cost = INFINITE;
                update_route_table();
                broadcast(send_socket);
            }
        }
        //Send routing control information periodically
        time_t time_now = time(NULL);
        if ((time_now - time_init) % CYCLE == 0)
        {
            broadcast(send_socket);
        }
        Sleep(SECOND);
    }
}
