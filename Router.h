/*
 * @Author: HZW ZJM CSS
 * @Date: 2021-05-11 22:46:35
 * @LastEditTime: 2021-05-14 10:16:35
 * @Description: 
 */
#include <iostream>
#include <vector>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include "Message.h"
#include "RouteTable.h"
#include "NextRoutes.h"
#include "Socket.h"
using namespace std;

#ifndef _ROUTER_H_
#define _ROUTER_H_
#define CYCLE 30
#define POSSIBLE_FAILURE_TIME 60
#define EXPIRATION_TIME 90
#define SECOND 1000
//Abstract router 
class Router
{
private:
    HANDLE control_thread_;
    HANDLE timer_thread_;
public:
    /**
     * @description: run the control thread
     * @param {*}
     * @return {*}
     */
    DWORD WINAPI Run(LPVOID lpParam);

    DWORD WINAPI Timer(LPVOID lpParam);

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
