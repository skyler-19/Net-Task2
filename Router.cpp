/*
 * @Author: your name
 * @Date: 2021-05-14 16:21:06
 * @LastEditTime: 2021-05-14 18:13:27
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Net\Router.cpp
 */
/*
 * @Author: your name
 * @Date: 2021-05-13 20:22:13
 * @LastEditTime: 2021-05-14 16:10:09
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Net\Router.cpp
 */
#include "Router.h"

DWORD WINAPI Router::control(LPVOID lpParam)
{
    Router* self  = (Router*)lpParam;
    DWORD ThrdID;
    self->timer_thread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)timer, (void *)self, 0, &ThrdID);
    while(1)
    {
        Message message = self->recv_message_and_update();
        if(message.message_type == Message::DATA_MESSAGE)
        {
            if(message.dest_ip_addr == self->local_ip_addr)
            {
                cout << "Data from " << inet_ntoa(*(in_addr*)&message.source_ip_addr) << " : ";
                cout << message.data << endl;
            }
            else
            {
                self->send_data_message(message);
            }
        }
        else//Control Message
        {
            if(self->algorithm == Router::DV &&self->update_net_state(message) && self->update_route_table())
            {
                self->broadcast();
            }
        }
    }
}

DWORD WINAPI Router::timer(LPVOID lpParam)
{
    Router* self = (Router*)lpParam;
    time_t time_init = time(NULL); //initial time: to decide the time to send update message(30s)
    while (1)
    {
        //Timeout detection 
        for (int i = 0; i < self->my_next_routers.size(); i++)
        {
            time_t time_now = time(NULL);
            time_t time_last = self->my_next_routers[i].last_update_time;
            if ((time_now - time_last) > EXPIRATION_TIME)
            {
                self->my_next_routers.erase(i); 
                self->update_route_table();
                self->broadcast();
            }
            else if ((time_now - time_last) > POSSIBLE_FAILURE_TIME)
            {
                self->my_next_routers[i].link_cost = INFINITE;
                self->update_route_table();
                self->broadcast();
            }
        }
        //Send routing control information periodically
        time_t time_now = time(NULL);
        if ((time_now - time_init) % CYCLE == 0)
        {
            self->broadcast();
        }
        Sleep(SECOND);
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

void Router::broadcast()
{
    for (int i = 0; i < my_route_table.size(); i++)
    {
        Message message;
        message.message_type = Message::ROUTE_CONTROL_MESSAGE;
        message.source_ip_addr = local_ip_addr;
        message.dest_ip_addr = my_route_table[i].dest_ip_addr;
        message.cost = my_route_table[i].cost;
        message.data[0] = '\0';
        broadcast_control_message(message);
    }
}

void Router::find_next_hop(long dest_ip_addr, long &next_hop_ip_addr, u_short next_hop_port)
{
    for (int i = 0; i < my_route_table.size(); i++)
    {
        if (my_route_table[i].dest_ip_addr == dest_ip_addr)
        {
            next_hop_ip_addr = my_route_table[i].next_hop_ip_addr;
            for (int j = 0; j < my_next_routers.size(); j++)
            {
                if (my_next_routers[j].ip_addr == next_hop_ip_addr)
                {
                    next_hop_port = my_next_routers[j].port;
                    return;
                }
            }
        }
    }
}

void Router::send_data_message(Message message)
{
    if (message.message_type == Message::DATA_MESSAGE)
    {
        long next_hop_ip_addr;
        u_short next_hop_port;
        find_next_hop(message.dest_ip_addr,next_hop_ip_addr,next_hop_port);
        send_message(send_socket,next_hop_ip_addr,next_hop_port,message);
    }
}
Message Router::recv_message_and_update()
{
    long ip_addr;
    Message message = recv_message(recv_socket,ip_addr);
    update_time(ip_addr);
    return message;
}

void Router::broadcast_control_message(Message message)
{
    for (int i = 0; i < my_next_routers.size(); i++)
    {
        long dest_ip_addr = my_next_routers[i].ip_addr;
        u_short dest_port = my_next_routers[i].port;
        send_message(send_socket, dest_ip_addr, dest_port, message);
    }
}
