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
#define INVALID_IP_ADDR 0
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
    LinkStateTable my_link_state_table;
    long local_ip_addr;
    SOCKET recv_socket;
    SOCKET send_socket;

    void initial()
    {
        WSA_initialization();
        recv_socket = create_recv_socket();
        send_socket = create_send_socket();
    }

    //start routing(receiving, forwarding, sending)
    void run();

    //Timeout detection and send routing information periodically
    void timer();

    //In DV, net_state is the next_routers,
    //In LS, net_state is the link state table 
    //return true if network state is changed
    bool update_net_state(Message message);

    //update the route table, return true if route table is changed
    bool update_route_table()
    {
        if(algorithm == DV)
        {
            return update_route_table_with_DV();
        }
        return update_route_table_with_LS();
    }

    bool update_route_table_with_DV();

    bool update_route_table_with_LS();

    //Remove unreachable items in route table
    void remove_unreachable_items();

    void update_time(long ip_addr);

    // broadcast control message, in DV is routetable, in LS is linkstate 
    void broadcast();

    // //broadcast poisoned reversed message 
    // void broadcast_poisoned();

    //get the next hop router according to the dest ip
    void find_next_hop(long dest_ip_addr, long &next_hop_ip_addr, u_short &next_hop_port);

    //find address and portand send it to the next hop
    void send_data_message(Message message);

    Message recv_message_and_update();

    //send control message to all the next hop routers
    void broadcast_control_message(Message message);

    void broadcast_next_router();

    //send control message to all the next hop routers except for the next hop 
    void broadcast_control_message(Message message,long next_hop_ip_addr);

    //add a adjacent router
    void add(long ip_addr,u_short port,int cost);

    //delete a adjacent router
    void delete_next_router(long ip_addr);

    void show_route_table();
    
    void show_next_routers();

    void show_link_state();
    
};

#endif
