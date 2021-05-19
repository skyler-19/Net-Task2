#ifndef _NEXTROUTES_H_
#define _NEXTROUTES_H_

#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <time.h>
#include <iomanip>
#include "Message.h"
#define LEN2 20
using namespace std;

//Encapsulate the list of all the next hop routers and the related operations
class NextRouters
{
public:
    typedef struct DistanceVector
    {
        long ip_addr;
        int distance;
        DistanceVector(long _ip_addr,int _distance)
        {
            ip_addr = _ip_addr;
            distance = _distance;
        }
    } DistanceVector;

    typedef vector<DistanceVector> DistanceVectorTable;

    //information of adjacent router
    typedef struct NextRouter
    {
        //bool valid;
        time_t last_update_time;
        long ip_addr;
        u_short port;
        int link_cost;
        DistanceVectorTable distance_vecor_table;
        NextRouter()
        {
            last_update_time = time(NULL);
        }
    } ValueType;

    //update the distance vector table of next routes, return true if distance vector is changed
    bool update(Message message);

    //update a adjacent router's last update time
    void update_time(long ip_addr);

    //add a new adjacent router
    void push(NextRouter next_router);

    //add a new adjacent router
    void push(long ip_addr, u_short port, int link_cost);

    //erase the corresponding router information
    void erase(int index);

    //return the number of adjacent router
    int size();

    //return a reference to the corresponding position in the table
    NextRouter &operator[](int index);

    void print();

private:
    vector<NextRouter> next_routers_;
};
#endif