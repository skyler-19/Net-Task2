#ifndef _LINKSTATETABLE_H_
#define _LINKSTATETABLE_H_
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "Message.h"
#define LEN3 20
#define MAX_COST 16
using namespace std;

//Include all link information (link is undirected)
//example:
// a<-1->b  a<-2->c  c<-3->d
//link state table:
//  [0]: {<a,b,1>, <a,c,2>}
//  [1]: {<b,a,1>}
//  [2]: {<c,a,2>, <c,d,3>}
//  [3]: {<d,c,3>}
class LinkStateTable
{
public:
    typedef struct LinkState
    {
        long source_ip_addr;
        long dest_ip_addr;
        int cost;
        LinkState(long _source_ip_addr, long _dest_ip_addr, int _cost)
        {
            source_ip_addr = _source_ip_addr;
            dest_ip_addr = _dest_ip_addr;
            cost = _cost;
        }
    }LinkState;
    //update the link state table, return true if it is changed
    bool update(Message message);

    //push a link and the reversed into the table (check if it has been added)
    //a link only needs to be added once
    void push(long source_ip_addr, long dest_ip_addr, int cost);

    //erase a link and the reversed (check if it has been added)
    void erase(long source_ip_addr, long dest_ip_addr);

    int size()
    {
        return link_state_table_.size();
    }

    void clear()
    {
        link_state_table_.clear();
    }

    //return true if the link is in the table
    bool is_in_table(long source_ip_addr, long dest_ip_addr);

    vector<LinkState> &operator[](int index);

    void print();
private:
    vector<vector<LinkState>> link_state_table_;
};

#endif