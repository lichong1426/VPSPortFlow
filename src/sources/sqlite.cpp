/*************************************************************************
	> File Name: sqlite.cpp
	> Author: 李冲
	> Mail:
	> Created Time: 2018年09月12日 星期三 16时13分28秒
 ************************************************************************/

#include"sqlite.h"
#include "logger.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
using std::string;
#define MAX_NUM_LEN 56
//回调函数
static int netflow_last_func(void *data, int argc, char **argv, char **azColName) {
	int i;
	if(data==nullptr)
	{
		LOG(ERROR) << "!****data指针为空****\r\n";
	    exit(-1);
	}

	//fprintf(stderr, "%s: ", (const char*)data);
	for (i = 0; i < argc; i++) {
		memset((char *)data,0, MAX_NUM_LEN);
		snprintf((char *)data, MAX_NUM_LEN, "%s",argv[i] ? argv[i] : "NULL");
	}
	return 0;
}

/*
static int netflow_sum_func(void *data, int argc, char **argv, char **azColName) {

	int i;
	fprintf(stderr, "%s: ", (const char*)data);
	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");

	snprintf((char *)data,sizeof(data),"%s",argv[0] ? argv[0] : "NULL");
	return 0;
}
*/

 //打开数据库,如果不存在则自动创建
bool SQLite::open_db(const string &filename)
{
	int ret;
	dbfile = filename;
	ret = sqlite3_open(filename.c_str(), &db);
	if (ret)
	{
		LOG(ERROR) << "无法打开数据库" << filename.data()<<":" << sqlite3_errmsg(db)<<"\r\n";
		printf("无法打开数据库%s:%s", filename.data(), sqlite3_errmsg(db));
		sqlite3_close(db);
		db = nullptr;
		return false;
	}
	LOG(INFO)<< "打开数据库" << filename.c_str() << "成功\r\n";
	return true;
}
/*===============================================================================
创建表
参数：
		tablename -- 表名
返回值：
		成功
说明：
	插入的数据为总流量
李冲  2018.09.15
===============================================================================*/
bool SQLite::create_table(const string &tablename)
{
	int ret;
	//将PRIMARY KEY指定为NULL，即可由引擎自动设定其值，引擎会设定为最大的rowid+1
	string table = " CREATE TABLE "+tablename+"(ID INTEGER PRIMARY KEY,TIME TEXT,NetFlow INTEGER);";
	if (db==nullptr)
	{
		open_db(dbfile);

	}
	if (db!=nullptr)//文件打开是成功的
	{
		ret = sqlite3_exec(db, table.c_str(), NULL, 0, &zErrMsg);
		if (ret != SQLITE_OK) {
			LOG(ERROR) << "SQLite创建表" << tablename.c_str() << "失败:" << zErrMsg << "\r\n"<<"命令:"<<table.c_str()<<"\r\n";
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			db = nullptr;
			return false;
		}
		LOG(INFO) << "SQLite创建表" << tablename.c_str() << "成功\r\n";
		return true;
	}
	return false;
}

/********************************************************获得一列的总和
string SQLite::get_netflow_sum(const string &tablename)
{
	string netflow_sum_cmd = "SELECT sum(NetFlow) FROM " + tablename;
	char netflow_sum[MAX_NUM_LEN];
	int ret;
	if (db == nullptr)
	{
		open_db(dbfile);
	}
	if (db != nullptr)
	{
		ret = sqlite3_exec(db, netflow_sum_cmd.data(), netflow_sum_func, netflow_sum, &zErrMsg);
		if (ret != SQLITE_OK) {
			LOG(ERROR) << "SQLite数据表 " << tablename.data() << " 错误:" << zErrMsg << "\r\n" << "命令:" << netflow_sum_cmd.data() << "\r\n";
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			db = nullptr;
			return "NULL";
		}
		sqlite3_free(zErrMsg);
// 		sqlite3_close(db);
// 		db=nullptr;
		return netflow_sum;
	}
	return "NULL";
}
*********************************************************/
/*===============================================================================
获得数据库中最近一条记录
参数：
		tablename -- 表名

返回值：
		字符串 流量,NULL则为空
李冲  2018.09.15
===============================================================================*/
string SQLite::get_netflow_last(const string &tablename)
{
	string cmd = "select * from "+ tablename+" order by id desc limit 0,1;";
	int ret;
	char netflow_last[MAX_NUM_LEN] = { 0 };
	if (db == nullptr)
	{
		open_db(dbfile);
	}
	if (db != nullptr)
	{
		ret = sqlite3_exec(db, cmd.data(), netflow_last_func, netflow_last, &zErrMsg);
		if (ret != SQLITE_OK) {
			LOG(ERROR) << "SQLite获得最近一条记录" << tablename.data() << "失败:" << zErrMsg << "\r\n" << "命令:" << cmd.data() << "\r\n";
			printf("SQLite获得最近一条记录失败 %s : %s\r\n", cmd.data(), zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			db = nullptr;
			return "0";
			//return false;
		}
		return netflow_last;
	}
    return "0";
}
/*===============================================================================
插入数据库数据
参数：
		tablename -- 表名
		id		  -- id号，为“NULL”则自增
		time      -- 时间
		netflow   -- 采集到的流量
返回值：
		c成功
说明：
	插入的数据为总流量
李冲  2018.09.15
===============================================================================*/
bool SQLite::insert(const string &tablename,string id,string time, string netflow)
{
	int ret;
	string last_sum_str;
	long long last_sum = 0;
	long long netflow_now = 0;

	last_sum_str =get_netflow_last(tablename);//获得最近一条记录
	last_sum = atoll(last_sum_str.data());//最近的一条记录
	netflow_now = atoll(netflow.data());//新采集的数据
	last_sum += netflow_now;//求和

	char netflow_str[MAX_NUM_LEN]={0};
	snprintf(netflow_str,sizeof(netflow_str),"%lld",last_sum);
	string insert_str = "INSERT INTO " + tablename + " VALUES (" + id + ",'" + time + "'," + netflow_str + ");";
	if (db == nullptr){
		open_db(dbfile);
	}
	if (db!=nullptr){
		ret = sqlite3_exec(db, insert_str.data(), NULL, 0, &zErrMsg);
		if (ret != SQLITE_OK) {
			LOG(ERROR) << "SQLite数据插入表" << tablename.c_str() << "失败:" << zErrMsg << "\r\n"  << "命令:" << insert_str.c_str() << "\r\n";
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			db = nullptr;
			return false;
		}
		LOG(INFO) << "SQLite数据插入表" << tablename.c_str() << "成功\r\n命令:"<< insert_str <<"\r\n";
		return true;
	}
	return false;
}


bool SQLite::close()
{
	sqlite3_close(db);
	db = nullptr;
	return true;
}

