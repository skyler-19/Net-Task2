/*
 * @Author: your name
 * @Date: 2021-05-13 10:11:04
 * @LastEditTime: 2021-05-13 11:29:02
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Net\DV\DV_Router.cpp
 */

#include "DV_Router.h"

void DV_Router::Run()
{
    
}

bool DV_Router::update_net_state(Message message)
{
    return my_next_routers_.update(message);
}

//traverse all distance vectors of all neighboring routers, 
//if it's cost less than current value or the dest ip hasn't been included,
//update the route table
bool DV_Router::update_route_table()
{
    bool is_changed = false;
    for(int i = 0;i < my_next_routers_.size();i++)
    {
        NextRouters::NextRouter &curr_router = my_next_routers_[i];
        NextRouters::DistanceVectorTable &curr_dis_vec_table = curr_router.distance_vecor_table;
        for(int j=0;j < curr_dis_vec_table.size();j++)
        {
            int curr_distance  = curr_dis_vec_table[j].distance;
            long curr_dest_ip = curr_dis_vec_table[j].ip_addr;
            bool is_find = false;
            for(int t = 0;t < my_route_table_.size();t++)
            {
                if(curr_dest_ip == my_route_table_[t].dest_ip_addr)
                {
                    is_find = true;
                    if(curr_router.link_cost + curr_distance < my_route_table_[t].cost)
                    {
                        is_changed = true;
                        my_route_table_[t].next_hop_ip_addr = curr_router.ip_addr;
                        my_route_table_[t].cost = curr_router.link_cost + curr_distance;                        
                    }
                    break;
                }
            }
            if(!is_find)
            {
                is_changed = true;
                my_route_table_.push(curr_dest_ip,curr_router.ip_addr,curr_router.link_cost + curr_distance);
            }
        }
    }
    return is_changed;
}