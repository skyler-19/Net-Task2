/*
 * @Author: your name
 * @Date: 2021-05-13 21:23:58
 * @LastEditTime: 2021-05-14 11:11:27
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Net\Socket.cpp
 */
#include "Socket.h"

extern RouteTable my_route_table;
extern NextRouters my_next_routers;

void WSA_initialization()
{
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata)) //version 2.2
    {
        cout << "initialization failed " << endl;
    }
}
//create a thread to receive message
SOCKET create_recv_socket()
{
    SOCKET recv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (recv_socket == INVALID_SOCKET)
    {
        cout << "socket fail" << endl;
        exit(1);
    }
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(RECV_PORT);
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(recv_socket, (sockaddr *)&local_addr, sizeof local_addr);
    return recv_socket;
}

//create a thread to send message
SOCKET create_send_socket()
{
    return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

//dest ip addr is in message
void send_data_message(SOCKET socket, Message message)
{
    if (message.message_type == Message::DATA_MESSAGE)
    {
        long next_hop_ip_addr;
        u_short next_hop_port;
        for (int i = 0; i < my_route_table.size(); i++)
        {
            if (my_route_table[i].dest_ip_addr == message.dest_ip_addr)
            {
                next_hop_ip_addr = my_route_table[i].next_hop_ip_addr;
                for (int j = 0; j < my_next_routers.size(); j++)
                {
                    if (my_next_routers[j].ip_addr == next_hop_ip_addr)
                    {
                        next_hop_port = my_next_routers[j].port;
                        break;
                    }
                }
                break;
            }
        }
        struct sockaddr_in toAddr;
        toAddr.sin_family = AF_INET;
        toAddr.sin_port = htons(next_hop_port);
        toAddr.sin_addr.s_addr = next_hop_ip_addr;
        int flag = sendto(socket, (char *)&message, sizeof(message), 0, (SOCKADDR *)&toAddr, sizeof(toAddr));
        if (flag == SOCKET_ERROR)
            printf("Data Message send failed!\nError:\n%d\n", WSAGetLastError());
    }
}

void send_control_message(SOCKET socket, long dest_ip_addr, u_short dest_port, Message message)
{
    if (message.message_type == Message::ROUTE_CONTROL_MESSAGE)
    {
        struct sockaddr_in toAddr;
        toAddr.sin_family = AF_INET;
        toAddr.sin_port = htons(dest_port);
        toAddr.sin_addr.s_addr = dest_ip_addr;
        int flag = sendto(socket, (char *)&message, sizeof(message), 0, (SOCKADDR *)&toAddr, sizeof(toAddr));
        if (flag == SOCKET_ERROR)
            printf("Control Message send failed!\nError:\n%d\n", WSAGetLastError());
    }
}

void broadcast_control_message(SOCKET socket, Message message)
{
    for (int i = 0; i < my_next_routers.size(); i++)
    {
        long dest_ip_addr = my_next_routers[i].ip_addr;
        u_short dest_port = my_next_routers[i].port;
        send_control_message(socket, dest_ip_addr, dest_port, message);
    }
}

Message recv_message(SOCKET recv_socket)
{
    Message message;
    memset(&message, 0, sizeof(message));
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    struct sockaddr_in from_addr;
    int addr_size = sizeof(from_addr);

    int flag = recvfrom(recv_socket, buffer, BUFFER_SIZE, 0, (SOCKADDR *)&from_addr, &addr_size);
    if (flag == SOCKET_ERROR)
        printf("Control Message send failed!\nError:\n%d\n", WSAGetLastError());
    memcpy((char *)&message, (const char *)buffer, sizeof(message));

    //update last update time
    my_next_routers.update_time(from_addr.sin_addr.S_un.S_addr);
    
    return message;
}
