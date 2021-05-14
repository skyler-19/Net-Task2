#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <cstring>
#include <windows.h>
#include <cmath>
#include "Message.h"
#include "NextRoutes.h"
#include "RouteTable.h"

#pragma comment(lib,"Ws2_32.lib ")
using namespace std;

NextRouters my_next_routers;
RouteTable my_route_table;


int main(){
    //Input the initial my_next_routers information
    cout << "Initialization..." << endl;
    //Message initial_message; 
    string my_name;     // This router's name
    cout << "Input the router name: ";
    cin >> my_name;
    getchar();

    //cmd format: next_ip_addr next_port link_cost 
    long next_ip_addr; 
    u_short next_port; 
    int link_cost;  
    int cnt=0;
    while(true){
        cout << "Router " << my_name << ">";
        string cmdin; //next_ip_addr or end;
        getline(cin, cmdin);
        if(cmdin == "exit"){
            cout << "Initialization finished!" << endl;
            break;
        }
        istringstream is(cmdin);
        string s;
        is >> s;
        next_ip_addr = inet_addr((char*)s.c_str());
        is >> s;
        next_port = atoi((char*)s.c_str());
        is >> s;
        link_cost = atoi((char*)s.c_str());
        my_next_routers.push(next_ip_addr, next_port, link_cost);
        my_route_table.push(next_ip_addr, next_ip_addr, link_cost);
        
    }
    my_route_table.print();
    //WSA initiallization

}

