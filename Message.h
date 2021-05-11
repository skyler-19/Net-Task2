/*
 * @Author: HZW ZJM CSS
 * @Date: 2021-05-11 10:38:21
 * @LastEditTime: 2021-05-11 19:35:29
 * @Description:Type of message transmitted
 */
#ifndef _ROUTETABLE_H_
#define _ROUTETABLE_H_

#include <iostream>
#include <string>
#include <winsock2.h>
using namespace std;

//the type of message
//  Data message:
//      message_type is DATA_MESSAGE,
//      dest_ip_addr is the ip address of destintation,
//      source_ip_addr is the ip address of source,
//      dest_port is the receive port of destintation,
//      cost is 0,
//      data is a short sentence
//  DV route control message:
//      message_type is ROUTE_CONTROL_MESSAGE,
//      the distance from 'source' to 'destination' is 'cost',
//      data is empty
//  LS route control message:
//      message_type is ROUTE_CONTROL_MESSAGE,
//      the link cost from 'source' to 'destination' is 'cost',
//      data is empty
struct Message
{
    enum MessageType
    {
        DATA_MESSAGE,
        ROUTE_CONTROL_MESSAGE
    };
    MessageType message_type;
    struct in_addr dest_ip_addr;
    struct in_addr source_ip_addr;
    u_short dest_port;
    int cost;
    string data;
};

#endif