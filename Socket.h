/*
 * @Author: your name
 * @Date: 2021-05-13 20:18:57
 * @LastEditTime: 2021-05-14 10:38:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Net\Socket.h
 */
#ifndef _SOCKET_H_
#define _SOCKET_H_
#include <winsock2.h>
#include <windows.h>
#include <bits/stdc++.h>
#include "Message.h"
#include "NextRoutes.h"
#include "RouteTable.h"
#include <string.h>
#pragma comment(lib, "ws2_32.lib")
#define RECV_PORT 8888
#define BUFFER_SIZE 1024

//Window Socket API initialization
void WSA_initialization();

//create a thread to receive message
SOCKET create_recv_socket();

//create a thread to send message
SOCKET create_send_socket();

//According to the destination ip, look up the routing table,
//find the ip of the next hop, then look up x, get the port number, 
//and send it to the next hop
void send_data_message(SOCKET socket, Message message); 

//send control message to a next hop router
void send_control_message(SOCKET sockst, long dest_ip_addr,u_short port, Message message); 

//send control message to all the next hop routers
void broadcast_control_message(SOCKET sockst, Message message);

//recv data message or control message from the socket
Message recv_message(SOCKET recv_socket);

#endif