#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include<iostream>
#include <string>
#include <vector>
#include "API.h"
#include "IndexManager.h"

using namespace std;
class Interpreter{
public:
	Interpreter();
	~Interpreter();
	int interpret(string s); //设置API 将command命令解释并连接到API接口
	string getFilename();
	void setAPI(API* apiInput);

private:
    API* api;
	string fileName;
	string getCommand(string s, int &st); //获取指令 一个一个地获取命令中的字符
};

#endif
