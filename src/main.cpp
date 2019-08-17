/*************************************************************************
        > File Name: main.cpp
        > Author:李冲
        > Mail:版本V1.0
        > Created Time: 2018年09月06日 星期四 20时13分04秒.
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

#include<iostream>
#include<string>
#include<vector>

#include"logger.h"
#include"sqlite.h"
#include"iptables.h"
#include "readconfig.h"

using namespace std;

int main(int argc,char **argv)
{
	Iptables iptables;
	SQLite sqlite;
	string netflow;
	string netflow_sum;
	vector<string> port_str;
	time_t tm;
	struct tm* t_tm;
	char time_str[56];//数据库文件名按月创建
	char dbfilename[56];//数据库文件名按月创建
	string info_log_filename = "./log/info_log.txt";
	string warn_log_filename = "./log/warn_log.txt";
	string error_log_filename= "./log/error_log.txt";

	//初始化LOG系统
    initLogger(info_log_filename, warn_log_filename, error_log_filename);
	LOG(INFO) << "*********程序版本 V1.0*********   编译日期2018-09-17\r\n";

	while (1)
	{

		//获取时间
		time(&tm);
		t_tm = localtime(&tm);
		sprintf(dbfilename, "./database/%4d_%02d.db", t_tm->tm_year + 1900, t_tm->tm_mon + 1);
		sqlite.open_db(dbfilename);

		//读取端口配置文件
		read_file(port_str, R"(./port.config)");
		//创建表并向iptables添加端口
		for (auto X : port_str)
		{
			cout << "配置端口:" << X << endl;
			string ct;
			ct = "PORT" + X;
			sqlite.create_table(ct);
			iptables.add_port(atoi(X.data()));
		}
		cout << endl;


		//创建数据库
		time(&tm);
		t_tm = localtime(&tm);
		sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d", t_tm->tm_year + 1900, t_tm->tm_mon + 1, t_tm->tm_mday, t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);

		for (auto X : port_str)
		{
			netflow = iptables.get_net_flow(atoi(X.data()));
			sqlite.insert("PORT" + X, "NULL", time_str, netflow);
			cout << X << "当前流量为：" << netflow << endl;
		}
		cout << endl;
		for (auto X : port_str)
		{
			netflow_sum = sqlite.get_netflow_last("PORT" + X);
			cout << X << "总流量为：" << netflow_sum << endl;
		}
		cout << "**************END************" << endl;

		/*======================   各项东西清零，析构   ============================*/
		port_str.clear();
		iptables.netfloe_reset();//清零外部 统计
		sleep(60*10);//睡眠10分钟

		//sqlite.insert("PORT6000","NULL", "2018-9-12", "666");
	}


    return 0;
}



