/*
 * @Author: HZW ZJM CSS
 * @Date: 2021-05-11 10:38:21
 * @LastEditTime: 2021-05-13 14:13:04
 * @Description: the list of all the next hop routers
 */
#ifndef _NEXTROUTES_H_
#define _NEXTROUTES_H_

#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <time.h>
#include "../Message.h"
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
        bool valid;
        time_t last_update_time;
        long ip_addr;
        u_short port;
        int link_cost;
        DistanceVectorTable distance_vecor_table;
        NextRouter()
        {
            last_update_time = time(NULL);
            valid = true;
        }
    } ValueType;

    /**
     * @description: 
     * @param {Message} message
     * @return {*}
     */
    bool update(Message message);

    /**
     * @description: add a new adjacent router
     * @param {NextRouter} next_router
     * @return {*}
     * @author:HZW
     */
    void push(NextRouter next_router);

    /**
     * @description: add a new adjacent router
     * @param {long} ip_addr
     * @param {u_short} port
     * @param {int} link_cost
     * @return {*}
     * @author:HZW
     */
    void push(long ip_addr, u_short port, int link_cost);

    /**
     * @description: return the number of adjacent router
     * @param {*}
     * @return {int }
     */
    int size();

    /**
     * @description: return a reference to the corresponding position in the table
     * @param {int} index
     * @return {NextRouter &}
     * @author:HZW
     */
    NextRouter &operator[](int index);

private:
    vector<NextRouter> next_routers_;
};
#endif