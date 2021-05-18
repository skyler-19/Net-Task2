#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <iostream>
#include <vector>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include "Message.h"
#include "RouteTable.h"
#include "NextRoutes.h"
#include "Socket.h"
#include "LinkStateTable.h"
#define CYCLE 30
#define POSSIBLE_FAILURE_TIME 60
#define EXPIRATION_TIME 90
#define SECOND 1000
#define MAX_DISTANCE 16
using namespace std;

//Abstract router
class Router
{
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

    void initial()
    {
        WSA_initialization();
        recv_socket = create_recv_socket();
        send_socket = create_send_socket();
    }

    /**
     * @description: start routing(receiving, forwarding, sending)
     * @param {*}
     * @return {*}
     */
    void run();

    /**
     * @description: Timeout detection and send routing information periodically
     * @param {*}
     * @return {*}
     */
    void timer();

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

    //Remove unreachable items in route table
    void remove_unreachable_items();

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

    void broadcast_split();

    /**
     * @description: get the next hop router according to the dest ip
     * @param {long} dest_ip_addr
     * @param {long} &next_hop_ip_addr
     * @param {u_short} next_hop_port
     * @return {*}
     */
    void find_next_hop(long dest_ip_addr, long &next_hop_ip_addr, u_short &next_hop_port);

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

    //@description: send control message to all the next hop routers except for the next hop 
    void broadcast_control_message(Message message,long next_hop_ip_addr);

    void add(long ip_addr,u_short port,int cost)
    {
        my_next_routers.push(ip_addr,port,cost);
        my_route_table.push(ip_addr,ip_addr,cost);
    }

    void show_route_table();
    
    void show_next_routers();
};

#endif
