/*
 * @Author: your name
 * @Date: 2021-05-13 20:18:57
 * @LastEditTime: 2021-05-14 18:07:56
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

//send message to a next hop router
void send_message(SOCKET sockst, long dest_ip_addr,u_short port, Message message); 

//recv data message or control message from the socket
Message recv_message(SOCKET recv_socket,long &from_ip_addr);

#endif