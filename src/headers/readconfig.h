/*************************************************************************
	> File Name: readconfig.h
	> Author: 李冲
	> Mail:
	> Created Time: 2018年09月13日 星期四 16时22分32秒
 ************************************************************************/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>

#define MAX_LINE_LEN 256
bool read_file(std::vector<std::string> &buff,const std::string &filename);
