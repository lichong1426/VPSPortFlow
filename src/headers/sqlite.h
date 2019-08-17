/*************************************************************************
	> File Name: sqlite.h
	> Author: 李冲
	> Mail:
	> Created Time: 2018年09月12日 星期三 16时13分42秒
 ************************************************************************/
#pragma once
#include"sqlite3.h"
#include <string>
using std::string;
class SQLite
{
public:
	bool open_db(const string &filename);
	bool create_table(const string &tablename);
	bool insert(const string &tablename,string id="NULL", string time="NULL", string netflow="0");
	//string get_netflow_sum(unsigned int port);
	//string get_netflow_sum(const string &tablename);
	string get_netflow_last(const string &tablename);//获得最近一次流量值，也就是最近一次记录
	//bool insert(const string &tablename, string id = "NULL", string time = "NULL",long long netflow);
	bool close(void);
private:
	string dbfile;//存储文件路径名字
	sqlite3 *db=nullptr;//文件指针
	char *zErrMsg = 0;//错误代码
};
