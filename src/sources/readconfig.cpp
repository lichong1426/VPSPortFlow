/*************************************************************************
	> File Name: readconfig.cpp
	> Author: 李冲
	> Mail:
	> Created Time: 2018年09月13日 星期四 16时22分37秒
 ************************************************************************/
#include "logger.h"
#include "readconfig.h"

using namespace std;
extern int errno;
bool read_file(vector<string> &buff, const string &filename)
{
	ifstream infile;
	infile.open(filename.data());
	if (!infile)
	{
		LOG(ERROR) << "port配置文件打开错误 " << filename.data()<<": " << strerror(errno) << "\r\n";
		printf("port配置文件打开错误 %s: %s\r\n", filename.data(), strerror(errno));
		return false;
	}
	char line[MAX_LINE_LEN + 2];

	while (!infile.eof())
	{
		line[0] = 0;
		if (!infile.getline(line, MAX_LINE_LEN))  continue;
		if (line[0] == 0 || line[0] == '#')   continue;
		if (line[strlen(line) - 1] == '\n')
		{
			line[strlen(line) - 1] = '\0';//去掉回车
		}
		string temp(line);
		buff.push_back(temp);
	}
	infile.close();
	return true;

}
