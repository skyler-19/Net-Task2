/*
 * @Author: HZW ZJM CSS
 * @Date: 2021-05-11 22:46:35
 * @LastEditTime: 2021-05-13 11:01:18
 * @Description: 
 */
#include <iostream>
#include <vector>
#include <string>
#include "Message.h"
#include "RouteTable.h"
using namespace std;

#ifndef _ROUTER_H_
#define _ROUTER_H_

//Abstract router 
class Router
{
private:
    HANDLE control_thread_;
public:
    RouteTable my_route_table_;
    /**
     * @description: run the control thread
     * @param {*}
     * @return {*}
     */
    virtual void Run() = 0;

    /**
     * @description: In DV, net_state is the next_routers,
     *               In LS, net_state is the link state table 
     * @param {Message} message
     * @return {bool} return true if network state is changed
     */
    virtual bool update_net_state(Message message) = 0;

    /**
     * @description: update the route table
     * @param {*}
     * @return {bool } return true if route table is changed
     */
    virtual bool update_route_table() = 0;
};


#endif
