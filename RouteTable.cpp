/*
 * @Author: your name
 * @Date: 2021-05-13 20:14:26
 * @LastEditTime: 2021-05-13 20:15:10
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Net\RouteTable.cpp
 */
#include "RouteTable.h"

void RouteTable::push(long dest_ip_addr, long next_hop_ip_addr, int cost)
{
    RouteTableEntry entry(dest_ip_addr, next_hop_ip_addr, cost);
    route_table_.push_back(entry);
}
int RouteTable::size()
{
    return route_table_.size();
}
RouteTable::RouteTableEntry &RouteTable::operator[](int index)
{
    if (index >= 0 && index < route_table_.size())
        return route_table_[index];
    cerr << "RouteTable: out of range" << endl;
    return *(RouteTableEntry *)NULL;
}
void RouteTable::print()
{
    cout << "Route Table: " << endl;
    cout << "Dest Ip Addr\t"
         << "Next Hop Ip Addr\t"
         << "Cost" << endl;
    for (int i = 0; i < 44; i++)
    {
        cout << "-";
    }
    cout << endl;
    for (int i = 0; i < route_table_.size(); i++)
    {
        cout << inet_ntoa(*((in_addr *)&route_table_[i].dest_ip_addr)) << "\t\t"
             << inet_ntoa(*((in_addr *)&route_table_[i].next_hop_ip_addr)) << "\t\t"
             << route_table_[i].cost << endl;
    }
}