/*************************************************************************
	> File Name: iptables.h
	> Author: 李冲
	> Mail:
	> Created Time: 2018年09月12日 星期三 15时19分50秒
 ************************************************************************/

#pragma once

#define MAXLINE 1024

class Iptables
{
public:
	bool add_port(unsigned int port);
	bool IsExist(unsigned int port);
	std::string get_net_flow(unsigned int port);
	bool netfloe_reset(void);

private:
	std::string convert(std::string s);

};
