/*************************************************************************
	> File Name: iptables.cpp
	> Author: 李冲
	> Mail:
	> Created Time: 2018年09月12日 星期三 15时20分02秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<cstring>
#include<sstream>
#include "iptables.h"
#include "logger.h"

extern int errno;
//添加一个端口进入iptables
bool Iptables::add_port(unsigned int port)
{
	FILE *Shell_fp;
	int Shell_ret = 0;
	char Shell_Result_buf[MAXLINE];
	char temp[128]={0};
	/*将要执行的命令写入buf*/
	char iptables_cmd[256]={0};
	if (IsExist(port))//端口存在，则退出
	{
		char temp[128];
	    snprintf(temp, sizeof(temp),"iptables添加端口失败:端口 %d 已存在\r\n",port);
		LOG(WARNING) << temp;
		return true;
	}
    snprintf(iptables_cmd, sizeof(iptables_cmd), "iptables -A INPUT -p tcp --dport %d\niptables -A OUTPUT -p tcp --sport %d",port,port);
	/*执行预先设定的命令，并读出该命令的标准输出*/
	Shell_fp = popen(iptables_cmd, "r");
	if (NULL == Shell_fp)
	{
		LOG(ERROR) <<"iptables添加端口失败:"<< strerror(errno)<<"\r\n"<<"命令:"<< iptables_cmd<<"\r\n";//错误信息写入ERROR
		printf("iptables添加端口失败:%s\r\n命令:%s\r\n", strerror(errno), iptables_cmd);
		exit(-1);
		//perror("iptables添加端口失败");
		return false;
	}
	while (fgets(Shell_Result_buf, sizeof(Shell_Result_buf), Shell_fp) != NULL)
	{
		LOG(INFO) << Shell_Result_buf;
	}

	/*等待命令执行完毕并关闭管道及文件指针*/
	Shell_ret = pclose(Shell_fp);
	if (-1 == Shell_ret)
	{
		LOG(ERROR) <<"关闭文件指针失败:"<< strerror(errno);//错误信息写入ERROR
		//perror("关闭文件指针失败");
		return false;
	}
	memset(temp,0,sizeof(temp));
	snprintf(temp, sizeof(temp), "iptables添加端口 %d 成功\r\n", port);
	LOG(INFO) << temp;
	return true;
}

//判断一个端口是否存在，存在返回true
bool Iptables::IsExist(unsigned int port)
{
	FILE *Shell_fp;
	char Shell_Result_buf[MAXLINE];
	/*将要执行的命令写入buf*/
	char iptables_cmd[128];
	snprintf(iptables_cmd, sizeof(iptables_cmd), " iptables -nvx -L | grep -w spt:%d", port);
	/*执行预先设定的命令，并读出该命令的标准输出*/
	Shell_fp = popen(iptables_cmd, "r");
	if (NULL == Shell_fp)
	{
		LOG(ERROR) << "iptables查询端口失败:" << strerror(errno) << "\r\n" << "命令:" << iptables_cmd << "\r\n";//错误信息写入ERROR
		printf("iptables查询端口失败:%s\r\n命令:%s\r\n", strerror(errno), iptables_cmd);
		exit(-1);
		//perror("iptables添加端口失败");
		return false;//查询失败默认不存在
	}
	while (fgets(Shell_Result_buf, sizeof(Shell_Result_buf), Shell_fp) != NULL)
	{
		if (strlen(Shell_Result_buf)>1)
		{
			pclose(Shell_fp);
			return true;//存在
		}
	}

	/*等待命令执行完毕并关闭管道及文件指针*/
	pclose(Shell_fp);
	return false;
}

//返回流量字符串
std::string Iptables::convert(std::string s)
{
	std::string num_str;
	std::stringstream input(s);
	input >> num_str;
	input >> num_str;
	return num_str;
	//return atoi(num_str.data());
}

/*===============================================================================
获得iptables的流量
参数：
		port -- 端口

返回值：
		成功

李冲  2018.09.15
===============================================================================*/
std::string Iptables::get_net_flow(unsigned int port)
{
	FILE *Shell_fp;
	int Shell_ret = 0;
	char Shell_Result_buf[MAXLINE];
	std::string netflow;//取得的流量
	/*将要执行的命令写入buf*/
	char iptables_cmd[256] = { 0 };

	if (!IsExist(port))//端口存在，则添加
	{
		LOG(WARNING) << "查询端口 " << port << "不存在，进行添加\r\n";
		add_port(port);
	}
	snprintf(iptables_cmd, sizeof(iptables_cmd), "iptables -nvx -L | grep -w spt:%d", port);
	/*执行预先设定的命令，并读出该命令的标准输出*/
	Shell_fp = popen(iptables_cmd, "r");
	if (NULL == Shell_fp)
	{
		LOG(ERROR) << "iptables查询流量失败:" << strerror(errno) << "\r\n" << "命令:" << iptables_cmd << "\r\n";//错误信息写入ERROR
		printf("iptables查询流量失败:%s\r\n命令:%s\r\n", strerror(errno), iptables_cmd);
		exit(-1);
	}

	fgets(Shell_Result_buf, sizeof(Shell_Result_buf), Shell_fp);//只要第一行
	if (strlen(Shell_Result_buf)<10)
	{
		LOG(ERROR) << "iptables查询端口 "<<port<<" 为空\r\n";
		return "0";//查询的为空
	}

    //printf("%s", Shell_Result_buf);
	netflow=convert(Shell_Result_buf);
	/*等待命令执行完毕并关闭管道及文件指针*/
	Shell_ret = pclose(Shell_fp);
	if (-1 == Shell_ret)
	{
		LOG(ERROR) << "关闭文件指针失败:" << strerror(errno);//错误信息写入ERROR
		//perror("关闭文件指针失败");
		return netflow;
	}
	return netflow;
}
/*===============================================================================
流量清零
李冲  2018.09.15
===============================================================================*/

bool Iptables::netfloe_reset(void) {
	FILE *Shell_fp;
	int Shell_ret = 0;
	/*将要执行的命令写入buf*/
	char iptables_cmd[256] = { 0 };
	snprintf(iptables_cmd, sizeof(iptables_cmd), "iptables -Z");
	/*执行预先设定的命令，并读出该命令的标准输出*/
	Shell_fp = popen(iptables_cmd, "r");
	if (NULL == Shell_fp)
	{
		LOG(ERROR) << "iptables流量清零失败:" << strerror(errno) << "\r\n" << "命令:" << iptables_cmd << "\r\n";//错误信息写入ERROR
		printf("iptables流量清零失败:%s\r\n命令:%s\r\n", strerror(errno), iptables_cmd);
		exit(-1);
	}
	/*等待命令执行完毕并关闭管道及文件指针*/
	Shell_ret = pclose(Shell_fp);
	if (-1 == Shell_ret)
	{
		LOG(ERROR) << "关闭文件指针失败:" << strerror(errno);//错误信息写入ERROR
		//perror("关闭文件指针失败");
		return false;
	}
	return true;
}
