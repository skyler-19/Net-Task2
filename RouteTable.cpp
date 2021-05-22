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

void RouteTable::erase(int index)
{
    if (index >= 0 && index < route_table_.size())
    {
        vector<RouteTableEntry>::iterator it = route_table_.begin();
        it += index;
        route_table_.erase(it);
    }
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
    cout << std::left << setw(LEN1) << "Dest Ip Addr"
         << std::left << setw(LEN1) << "Next Hop Ip Addr"
         << std::left << setw(LEN1) << "Cost" << endl;
    for (int i = 0; i < 3 * LEN1; i++)
    {
        cout << "-";
    }
    cout << endl;
    for (int i = 0; i < route_table_.size(); i++)
    {
        cout << std::left << setw(LEN1) << inet_ntoa(*((in_addr *)&route_table_[i].dest_ip_addr))
             << std::left << setw(LEN1) << inet_ntoa(*((in_addr *)&route_table_[i].next_hop_ip_addr))
             << std::left << setw(LEN1) << route_table_[i].cost << endl;
    }
}

void RouteTable::clear()
{
    route_table_.clear();
}