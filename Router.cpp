#include "Router.h"

void Router::run()
{
    broadcast_poisoned();
    //broadcast();
    while (1)
    {
        Message message = recv_message_and_update();

        if (message.message_type == Message::DATA_MESSAGE)
        {
            if (message.dest_ip_addr == local_ip_addr)
            {
                cout << endl
                     << "Data from " << inet_ntoa(*(in_addr *)&message.source_ip_addr) << " : ";
                cout << message.data << endl;
            }
            else
            {
                send_data_message(message);
            }
        }
        else //Control Message
        {
            if (message.dest_ip_addr != local_ip_addr)
            {
                if (algorithm == Router::DV && update_net_state(message) && update_route_table())
                {
                    update_route_table();
                    broadcast_poisoned();
                    remove_unreachable_items();
                }
                else if (algorithm == Router::LS && update_net_state(message))
                {
                    broadcast_control_message(message);
                    update_route_table();
                }
            }
        }
    }
}

void Router::timer()
{
    time_t time_init = time(NULL); //initial time: to decide the time to send update message(30s)
    while (1)
    {
        //Timeout detection
        for (int i = 0; i < my_next_routers.size(); i++)
        {
            time_t time_now = time(NULL);
            time_t time_last = my_next_routers[i].last_update_time;
            if (my_next_routers[i].link_cost == MAX_DISTANCE && (time_now - time_last) > EXPIRATION_TIME)
            {
                my_next_routers.erase(i);
                if (update_route_table())
                    broadcast_poisoned();
            }
            else if ((time_now - time_last) > POSSIBLE_FAILURE_TIME)
            {
                my_next_routers[i].link_cost = MAX_DISTANCE;
                // Message message;
                // message.message_type = Message::ROUTE_CONTROL_MESSAGE;
                // message.source_ip_addr = local_ip_addr;
                // message.dest_ip_addr = my_next_routers[i].ip_addr;
                // message.cost = MAX_COST;
                // my_link_state_table.update(message);
                if (update_route_table())
                {
                    update_route_table();
                    broadcast_poisoned();
                    remove_unreachable_items();
                }
            }
        }
        //Send routing control information periodically
        time_t time_now = time(NULL);
        if (time_now - time_init > CYCLE)
        {
            time_init = time_now;
            broadcast_poisoned();
            //broadcast_next_router();
            remove_unreachable_items();
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
                    if (my_route_table[t].next_hop_ip_addr == curr_router.ip_addr)
                    {
                        if (curr_router.link_cost + curr_distance != my_route_table[t].cost)
                        {
                            is_changed = true;
                            my_route_table[t].cost = curr_router.link_cost + curr_distance;
                            if (my_route_table[t].cost >= MAX_DISTANCE)
                            {
                                my_route_table[t].cost = MAX_DISTANCE;
                                is_changed = false;
                            }
                        }
                    }
                    else if (curr_router.link_cost + curr_distance < my_route_table[t].cost)
                    {
                        is_changed = true;
                        my_route_table[t].next_hop_ip_addr = curr_router.ip_addr;
                        my_route_table[t].cost = curr_router.link_cost + curr_distance;
                        if (my_route_table[t].cost >= MAX_DISTANCE)
                        {
                            my_route_table[t].cost = MAX_DISTANCE;
                            is_changed = false;
                        }
                    }
                    break;
                }
            }
            if (!is_find && curr_distance + curr_router.link_cost < MAX_DISTANCE)
            {
                is_changed = true;
                my_route_table.push(curr_dest_ip, curr_router.ip_addr, curr_router.link_cost + curr_distance);
            }
        }
    }
    if (is_changed)
        show_route_table();
    return is_changed;
}
void Router::update_time(long ip_addr)
{
    my_next_routers.update_time(ip_addr);
}

void Router::remove_unreachable_items()
{
    for (int i = 0; i < my_route_table.size(); i++)
    {
        if (my_route_table[i].cost >= MAX_DISTANCE)
            my_route_table.erase(i);
    }
}

void Router::broadcast()
{
    for (int i = 0; i < my_route_table.size(); i++)
    {
        //cout << "route table" << i << endl;
        Message message;
        message.message_type = Message::ROUTE_CONTROL_MESSAGE;
        message.source_ip_addr = local_ip_addr;
        message.dest_ip_addr = my_route_table[i].dest_ip_addr;
        message.cost = my_route_table[i].cost;
        message.data[0] = '\0';
        broadcast_control_message(message);
    }
}

void Router::broadcast_poisoned()
{
    for (int i = 0; i < my_route_table.size(); i++)
    {
        //cout << "route table" << i << endl;
        Message message;
        message.message_type = Message::ROUTE_CONTROL_MESSAGE;
        message.source_ip_addr = local_ip_addr;
        message.dest_ip_addr = my_route_table[i].dest_ip_addr;
        message.cost = my_route_table[i].cost;
        message.data[0] = '\0';
        broadcast_control_message(message, my_route_table[i].next_hop_ip_addr);
    }
}

// void Router::broadcast_next_router()
// {
//     Message message;
//     message.message_type = Message::ROUTE_CONTROL_MESSAGE;
//     message.source_ip_addr = local_ip_addr;
//     message.dest_ip_addr = local_ip_addr;
//     message.cost = 0;
//     for (int i = 0; i < my_next_routers.size(); i++)
//     {
//         send_message(send_socket, my_next_routers[i].ip_addr, my_next_routers[i].port, message);
//     }
// }

void Router::find_next_hop(long dest_ip_addr, long &next_hop_ip_addr, u_short &next_hop_port)
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

void Router::add(long ip_addr, u_short port, int cost)
{
    my_next_routers.push(ip_addr, port, cost);
    my_route_table.push(ip_addr, ip_addr, cost);
    my_link_state_table.push(local_ip_addr, ip_addr, cost);
}

void Router::delete_next_router(long ip_addr)
{
    for (int i = 0; i < my_next_routers.size(); i++)
    {
        if (my_next_routers[i].ip_addr == ip_addr)
        {
            my_next_routers[i].link_cost = MAX_COST;
            break;
        }
    }
    update_route_table();
    update_route_table();
    broadcast_poisoned();
    remove_unreachable_items();
}

void Router::send_data_message(Message message)
{
    if (message.message_type == Message::DATA_MESSAGE)
    {
        long next_hop_ip_addr;
        u_short next_hop_port;
        find_next_hop(message.dest_ip_addr, next_hop_ip_addr, next_hop_port);
        send_message(send_socket, next_hop_ip_addr, next_hop_port, message);
    }
}

Message Router::recv_message_and_update()
{
    long ip_addr;
    Message message = recv_message(recv_socket, ip_addr);
    update_time(ip_addr);
    return message;
}

void Router::broadcast_control_message(Message message)
{
    for (int i = 0; i < my_next_routers.size(); i++)
    {
        //cout << "next route" << i << endl;
        if(my_next_routers[i].link_cost >= MAX_COST) continue;
        long dest_ip_addr = my_next_routers[i].ip_addr;
        u_short dest_port = my_next_routers[i].port;
        send_message(send_socket, dest_ip_addr, dest_port, message);
    }
}

void Router::broadcast_control_message(Message message, long next_hop_ip_addr)
{
    for (int i = 0; i < my_next_routers.size(); i++)
    {
        if(my_next_routers[i].link_cost >= MAX_COST) continue;
        long dest_ip_addr = my_next_routers[i].ip_addr;
        u_short dest_port = my_next_routers[i].port;
        if (dest_ip_addr != next_hop_ip_addr)
            send_message(send_socket, dest_ip_addr, dest_port, message);
        else
        {
            Message poisoned_reverse_message = message;
            poisoned_reverse_message.cost = MAX_COST;
            send_message(send_socket, dest_ip_addr, dest_port, poisoned_reverse_message);
        }
    }
}

void Router::show_route_table()
{
    my_route_table.print();
}

void Router::show_next_routers()
{
    my_next_routers.print();
}