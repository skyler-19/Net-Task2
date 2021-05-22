# Task2 Virtual Routing

## 一、任务要求

基于DV和LS算法分别实现应用层上的虚拟路由，包括数据报文的转发以及路由表的生成。

## 二、实验环境

+ 系统：window
+ 语言：C++
+ 编译方式：`g++ *.cpp -o [name] -lwsock32`

## 三、文件结构

+ Message.h：传输报文类
+ RouteTable.h：路由表类
+ NextRoutes.h：相邻路由器列表类
+ LinkStateTable.h: 链路信息表类
+ Router.h: 路由器类
+ Socket.h: 封装通信函数
+ main.cpp: 模拟命令行窗口，接收指令并初始化

## 四、实验拓扑

![拓扑](https://github.com/skyler-19/Net-Task2/blob/master/data/%E6%8B%93%E6%89%91.png)
