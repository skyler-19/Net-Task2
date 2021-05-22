#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <iostream>
#include <string>
#include <winsock2.h>
#define DATA_SIZE 256
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
    long dest_ip_addr;
    long source_ip_addr;
    int cost;
    char data[DATA_SIZE];
    Message operator=(const Message& message)
    {
        dest_ip_addr = message.dest_ip_addr;
        source_ip_addr = message.source_ip_addr;
        cost = message.cost;
        memcpy(data,(const char*)message.data,sizeof(data));
    }
};
#endif