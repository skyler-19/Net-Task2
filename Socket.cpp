#include "Socket.h"

void WSA_initialization()
{
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata)) //version 2.2 
    {
        cout << "initialization failed " << endl;
    }
}

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
    bind(recv_socket, (sockaddr *)&local_addr, sizeof(local_addr));
    return recv_socket;
}

SOCKET create_send_socket()
{
    return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

void send_message(SOCKET socket, long dest_ip_addr, u_short dest_port, Message message)
{
    struct sockaddr_in toAddr;
    toAddr.sin_family = AF_INET;
    toAddr.sin_port = htons(dest_port);
    toAddr.sin_addr.s_addr = dest_ip_addr;
    int flag = sendto(socket, (char *)&message, sizeof(message), 0, (SOCKADDR *)&toAddr, sizeof(toAddr));
    cout << "\n[send to " << inet_ntoa(*((in_addr *)&dest_ip_addr)) << "]\t";
    cout << "type: " << message.message_type << "\t";
    cout << "source ip: " << inet_ntoa(*((in_addr *)&message.source_ip_addr)) << "\t";
    cout << "dest ip: " << inet_ntoa(*((in_addr *)&message.dest_ip_addr)) << "\t";
    cout << "cost: " << message.cost << endl;
    if (flag == SOCKET_ERROR)
        printf("Message send failed!\nError:\n%d\n", WSAGetLastError());
}

Message recv_message(SOCKET recv_socket, long &from_ip_addr)
{
    Message message;
    memset(&message, 0, sizeof(message));
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    struct sockaddr_in from_addr;
    int addr_size = sizeof(from_addr);
    int flag = recvfrom(recv_socket, buffer, BUFFER_SIZE, 0, (SOCKADDR *)&from_addr, &addr_size);
    if (flag == SOCKET_ERROR)
        printf("Message recv failed!\nError:\n%d\n", WSAGetLastError());
    memcpy((char *)&message, (const char *)buffer, sizeof(message));
    from_ip_addr = from_addr.sin_addr.S_un.S_addr;
    cout << "\n[recv from " << inet_ntoa(*((in_addr *)&from_ip_addr)) << "]\t";
    cout << "type: " << message.message_type << "\t";
    cout << "source ip: " << inet_ntoa(*((in_addr *)&message.source_ip_addr)) << "\t";
    cout << "dest ip: " << inet_ntoa(*((in_addr *)&message.dest_ip_addr)) << "\t";
    cout << "cost: " << message.cost << endl;
    return message;
}
