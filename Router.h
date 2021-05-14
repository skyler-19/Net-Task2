/*
 * @Author: HZW ZJM CSS
 * @Date: 2021-05-11 22:46:35
 * @LastEditTime: 2021-05-14 18:06:24
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
    Router *self;

public:
    enum ALGORITHM_TYPE
    {
        DV,
        LS
    };
    ALGORITHM_TYPE algorithm;
    RouteTable my_route_table;
    NextRouters my_next_routers;
    long local_ip_addr;
    SOCKET recv_socket;
    SOCKET send_socket;
    HANDLE control_thread_;
    HANDLE timer_thread_;

    void initial(ALGORITHM_TYPE _algorithm,long _local_ip_addr)
    {
        algorithm = _algorithm;
        local_ip_addr = _local_ip_addr;
        self = this;
        WSA_initialization();
        SOCKET recv_socket = create_recv_socket();
        SOCKET send_socket = create_send_socket();
        broadcast();
    }
    /**
     * @description: run the control thread
     * @param {Router*}
     * @return {*}
     */
    static DWORD WINAPI control(LPVOID lpParam);

    //create control thread and run
    void run()
    {
        DWORD ThrdID; //thread ID
        control_thread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)control, (void *)self, 0, &ThrdID);
    }

    /**
     * @description: Timeout detection and send routing information periodically
     * @param {Rouer*} lpParam
     * @return {*}
     */
    static DWORD WINAPI timer(LPVOID lpParam);

    /**
     * @description: In DV, net_state is the next_routers,
     *               In LS, net_state is the link state table 
     * @param {Message} message
     * @return {bool} return true if network state is changed
     */
    bool update_net_state(Message message);

    /**
     * @description: update the route table
     * @param {*}
     * @return {bool } return true if route table is changed
     */
    bool update_route_table();

    void update_time(long ip_addr)
    {
        my_next_routers.update_time(ip_addr);
    }

    /**
     * @description: broadcast control message, in DV is routetable, in LS is linkstate 
     * @param {*}
     * @return {*}
     */
    void broadcast();

    /**
     * @description: get the next hop router according to the dest ip
     * @param {long} dest_ip_addr
     * @param {long} &next_hop_ip_addr
     * @param {u_short} next_hop_port
     * @return {*}
     */
    void find_next_hop(long dest_ip_addr, long &next_hop_ip_addr, u_short next_hop_port);

    /**
     * @description: find address and portand send it to the next hop
     * @param {SOCKET} socket
     * @param {Message} message
     * @return {*}
     */
    void send_data_message(Message message);

    Message recv_message_and_update();

    /**
     * @description: send control message to all the next hop routers
     * @param {SOCKET} sockst
     * @param {Message} message
     * @return {*}
     */
    void broadcast_control_message(Message message);
};

#endif
