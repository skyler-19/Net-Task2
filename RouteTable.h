/*
 * @Author: HZW ZJM CSS
 * @Date: 2021-05-11 20:53:32
 * @LastEditTime: 2021-05-13 11:01:02
 * @Description: 
 */

#ifndef _ROUTETABLE_H_
#define _ROUTETABLE_H_

#include <iostream>
#include <vector>
using namespace std;

class RouteTable
{
public:
    typedef struct RouteTableEntry
    {
        long dest_ip_addr;
        long next_hop_ip_addr;
        int cost;
        RouteTableEntry(long _dest_ip_addr, long _next_hop_ip_addr, int _cost)
        {
            dest_ip_addr = _dest_ip_addr;
            next_hop_ip_addr = _next_hop_ip_addr;
            cost = _cost;
        }
    } RouteTableEntry;
    void push(long dest_ip_addr, long next_hop_ip_addr, int cost)
    {
        RouteTableEntry entry(dest_ip_addr, next_hop_ip_addr, cost);
        route_table_.push_back(entry);
    }
    RouteTableEntry &operator[](int index)
    {
        if (index >= 0 && index < route_table_.size())
            return route_table_[index];
        cerr << "RouteTable: out of range" << endl;
        return *(RouteTableEntry *)NULL;
    }
    void print();
private:
    vector<RouteTableEntry> route_table_;
};

#endif
