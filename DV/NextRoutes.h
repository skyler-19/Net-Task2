#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <string>
#include <vector>
#include <time.h>
#include <Message.h>
using namespace std;
#ifndef _NEXTROUTES_H_
#define _NEXTROUTES_H_

class NextRouters
{
private:
    vector<NextRouter> next_routers_;

public:
    typedef vector<pair<struct in_addr, u_short>> DistanceVector;
    typedef struct NextRouter
    {
        bool valid;
        time_t last_update_time;
        struct in_addr ip_addr;
        u_short port;
        int link_cost;
        DistanceVector distance_vecor;
    } ValueType;
    void update(Message message);
    void push(NextRouter next_router);
    NextRouter &operator[](int index)
    {
        if (index >= 0 && index < next_routers.size())
            return next_routers_[index];
        cerr << "NextRouters: out of range" << endl;
        return *(NextRouter *)NULL;
    }
};
#endif