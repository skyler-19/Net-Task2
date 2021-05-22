#include "Router.h"

void Router::run()
{
    broadcast();
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
            if (algorithm == Router::DV && message.dest_ip_addr != local_ip_addr && update_net_state(message) && update_route_table())
            {
                update_route_table();
                broadcast();
                remove_unreachable_items();
            }
            if (algorithm == Router::LS && update_net_state(message))
            {
                broadcast_control_message(message);
                update_route_table();
                show_route_table();
                remove_unreachable_items();
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
                if (algorithm == DV && update_route_table())
                {
                    broadcast();
                }
            }
            else if ((time_now - time_last) > POSSIBLE_FAILURE_TIME)
            {
                delete_next_router(my_next_routers[i].ip_addr);
            }
        }
        //Send routing control information periodically
        time_t time_now = time(NULL);
        if (time_now - time_init > CYCLE)
        {
            time_init = time_now;
            broadcast();
            remove_unreachable_items();
        }
        Sleep(SECOND);
    }
}

bool Router::update_net_state(Message message)
{
    if (algorithm == DV)
    {
        return my_next_routers.update(message);
    }
    if (message.dest_ip_addr == local_ip_addr)
    {
        if (message.cost == MAX_COST)
        {
            delete_next_router(message.source_ip_addr);
        }
        return false;
    }
    return my_link_state_table.update(message);
}

//traverse all distance vectors of all neighboring routers,
//if it's cost less than current value or the dest ip hasn't been included,
//update the route table
bool Router::update_route_table_with_DV()
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
                            if (my_route_table[t].cost > MAX_DISTANCE)
                            {
                                my_route_table[t].cost = MAX_DISTANCE;
                            }
                        }
                    }
                    else if (curr_router.link_cost + curr_distance < my_route_table[t].cost)
                    {
                        is_changed = true;
                        my_route_table[t].next_hop_ip_addr = curr_router.ip_addr;
                        my_route_table[t].cost = curr_router.link_cost + curr_distance;
                        if (my_route_table[t].cost > MAX_DISTANCE)
                        {
                            my_route_table[t].cost = MAX_DISTANCE;
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

bool Router::update_route_table_with_LS()
{
    my_route_table.clear();
    //initialization
    for (int i = 0; i < my_link_state_table.size(); i++)
    {
        if (!my_link_state_table[i].empty() && my_link_state_table[i][0].source_ip_addr != local_ip_addr)
        {
            my_route_table.push(my_link_state_table[i][0].source_ip_addr, INVALID_IP_ADDR, MAX_COST);
        }
    }
    vector<bool> is_determined(my_route_table.size(), false);
    //update adjacent router
    int local_index = my_link_state_table.get_index(local_ip_addr);
    for (int i = 0; i < my_link_state_table[local_index].size(); i++)
    {
        for (int j = 0; j < my_route_table.size(); j++)
        {
            if (my_link_state_table[local_index][i].dest_ip_addr == my_route_table[j].dest_ip_addr)
            {
                my_route_table[j].next_hop_ip_addr = my_route_table[j].dest_ip_addr;
                my_route_table[j].cost = my_link_state_table[local_index][i].cost;
            }
        }
    }
    //Dijkstra, find the shortest path
    while (1)
    {
        //find the nearest route
        int min_cost = MAX_COST;
        int nearest_router_index = -1;
        long nearest_router_ip_addr = 0;
        long nearest_router_next_hop = 0;
        for (int i = 0; i < my_route_table.size(); i++)
        {
            if (!is_determined[i] && my_route_table[i].cost < min_cost)
            {
                min_cost = my_route_table[i].cost;
                nearest_router_index = i;
                nearest_router_ip_addr = my_route_table[i].dest_ip_addr;
                nearest_router_next_hop = my_route_table[i].next_hop_ip_addr;
            }
        }
        //no matching router
        if (nearest_router_index == -1)
            break;
        //update the information of the nearest router
        is_determined[nearest_router_index] = true;
        //find the nearest router
        for (int i = 0; i < my_link_state_table.size(); i++)
        {
            if (!my_link_state_table[i].empty() && my_link_state_table[i][0].source_ip_addr == nearest_router_ip_addr)
            {
                vector<LinkStateTable::LinkState> &curr_link_table = my_link_state_table[i];
                //traverse all the link
                for (int j = 0; j < curr_link_table.size(); j++)
                {
                    int curr_link_cost = curr_link_table[j].cost;
                    long curr_dest_ip_addr = curr_link_table[j].dest_ip_addr;
                    //find the corresponding entry
                    for (int t = 0; t < my_route_table.size(); t++)
                    {
                        if (!is_determined[t] && my_route_table[t].dest_ip_addr == curr_dest_ip_addr)
                        {
                            if (min_cost + curr_link_cost < my_route_table[t].cost)
                            {
                                my_route_table[t].next_hop_ip_addr = nearest_router_next_hop;
                                my_route_table[t].cost = min_cost + curr_link_cost;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
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
    if (algorithm == DV)
    {
        for (int i = 0; i < my_route_table.size(); i++)
        {
            Message message;
            message.message_type = Message::ROUTE_CONTROL_MESSAGE;
            message.source_ip_addr = local_ip_addr;
            message.dest_ip_addr = my_route_table[i].dest_ip_addr;
            message.cost = my_route_table[i].cost;
            message.data[0] = '\0';
            broadcast_control_message(message, my_route_table[i].next_hop_ip_addr);
        }
    }
    else if (algorithm == LS)
    {
        for (int i = 0; i < my_next_routers.size(); i++)
        {
            if (my_next_routers[i].link_cost != MAX_COST)
            {
                Message message;
                message.message_type = Message::ROUTE_CONTROL_MESSAGE;
                message.source_ip_addr = local_ip_addr;
                message.dest_ip_addr = my_next_routers[i].ip_addr;
                message.cost = my_next_routers[i].link_cost;
                message.data[0] = '\0';
                broadcast_control_message(message);
            }
        }
    }
}

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
    if (algorithm == DV)
    {
        if (update_route_table())
        {
            update_route_table();
            broadcast();
            remove_unreachable_items();
        }
    }
    else if (algorithm == LS)
    {
        Message message;
        message.message_type = Message::ROUTE_CONTROL_MESSAGE;
        message.source_ip_addr = local_ip_addr;
        message.dest_ip_addr = ip_addr;
        message.cost = MAX_COST;
        if (update_net_state(message))
        {
            broadcast_control_message(message);
            show_link_state();
            update_route_table();
        }
    }
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
        if (my_next_routers[i].link_cost >= MAX_COST)
            continue;
        long dest_ip_addr = my_next_routers[i].ip_addr;
        u_short dest_port = my_next_routers[i].port;
        send_message(send_socket, dest_ip_addr, dest_port, message);
    }
}

void Router::broadcast_control_message(Message message, long next_hop_ip_addr)
{
    for (int i = 0; i < my_next_routers.size(); i++)
    {
        if (my_next_routers[i].link_cost >= MAX_COST)
            continue;
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

void Router::show_link_state()
{
    my_link_state_table.print();
}