#include <iostream>
#include <string>
#include <winsock2.h>
using namespace std;

struct Message
{
    enum MessageType{DATA_MESSAGE,ROUTE_CONTROL_MESSAGE};
    MessageType message_type_;
    struct in_addr dest_ip_addr;
    struct in_addr source_ip_addr;
    u_short dest_port;
    string Data;



};