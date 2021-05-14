/*
 * @Author: HZW ZJM CSS
 * @Date: 2021-05-11 10:38:21
 * @LastEditTime: 2021-05-14 11:06:59
 */
#include "NextRoutes.h"

//find the corresponding router, if the distance vector exist, update its value, 
//else put it into the distance_vector_table
//and indicates that the current routing information has changed
bool NextRouters::update(Message message)
{
    bool is_changed = false;
    if (message.message_type == Message::ROUTE_CONTROL_MESSAGE)
    {
        for (int i = 0; i < next_routers_.size(); i++)
        {
            if (next_routers_[i].ip_addr == message.source_ip_addr)
            {
                bool find_flag = false;
                DistanceVectorTable &curr_vector = next_routers_[i].distance_vecor_table;
                for (int j = 0; j < curr_vector.size(); j++)
                {
                    if (curr_vector[j].ip_addr == message.dest_ip_addr)
                    {
                        find_flag = true;
                        if(curr_vector[j].distance == message.cost)
                        {
                            is_changed = true;
                        }
                        curr_vector[j].distance = message.cost;
                        break;
                    }
                }
                if (!find_flag)
                {
                    DistanceVector distance_vector(message.dest_ip_addr,message.cost);
                    curr_vector.push_back(distance_vector);
                    is_changed = true;
                }
                break;
            }
        }
    }
    return is_changed;
}

void NextRouters::update_time(long ip_addr)
{
    for(int i = 0;i < next_routers_.size();i++)
    {
        if(next_routers_[i].ip_addr == ip_addr)
        {
            next_routers_[i].last_update_time = time(NULL);
            break;
        }
    }
}

void NextRouters::push(NextRouter next_router)
{
    next_router.last_update_time = time(NULL);
    //next_router.valid = true;
    next_routers_.push_back(next_router);
}

void NextRouters::push(long _ip_addr, u_short _port, int _link_cost)
{
    NextRouter next_router;
    next_router.last_update_time = time(NULL);
    //next_router.valid = true;
    next_router.ip_addr = _ip_addr;
    next_router.port = _port;
    next_router.link_cost = _link_cost;
    next_routers_.push_back(next_router);
}

void NextRouters::erase(int index)
{
    if (index >= 0 && index < next_routers_.size())
    {
        vector<NextRouter>::iterator it = next_routers_.begin();
        it += index;
        next_routers_.erase(it);
    }
}


NextRouters::NextRouter &NextRouters::operator[](int index)
{
    if (index >= 0 && index < next_routers_.size())
        return next_routers_[index];
    cerr << "NextRouters: out of range" << endl;
    return *(NextRouter *)NULL;
}

int NextRouters::size()
{
    return next_routers_.size();
}