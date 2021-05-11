/*
 * @Author: HZW ZJM CSS
 * @Date: 2021-05-11 20:53:32
 * @LastEditTime: 2021-05-11 22:29:07
 * @Description: 
 */

#ifndef _ROUTETABLE_H_
#define _ROUTETABLE_H_

#include <iostream>
#include <winsock2.h>
#include <vector>
using namespace std;

typedef struct RouteTableEntry
{
    long dest_ip_addr;
    long next_hop_ip_addr;
    int cost;
}RouteTableEntry;

typedef vector<RouteTableEntry> RouteTable;

#endif
