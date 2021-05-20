#ifndef _ROUTETABLE_H_
#define _ROUTETABLE_H_

#include <iostream>
#include <vector>
#include <winsock2.h>
#include <iomanip>
using namespace std;
#define LEN1 20
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
    void push(long dest_ip_addr, long next_hop_ip_addr, int cost);
    int size();
    void erase(int index);
    RouteTableEntry &operator[](int index);
    void print();
    void clear();
private:
    vector<RouteTableEntry> route_table_;
};

#endif
