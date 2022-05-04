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
<<<<<<< HEAD
	int interpret(string s); //将命令解释给API
=======
	int interpret(string s); //设置API
>>>>>>> 35cd068e5018e4c6a538ed8168b735d28635db80
	string getFilename();
	void setAPI(API* apiInput);

private:
    API* api;
	string fileName;
<<<<<<< HEAD
	string getCommand(string s, int &st); //得到命令中的每个单词
=======
	string getCommand(string s, int &st); //获取指令
>>>>>>> 35cd068e5018e4c6a538ed8168b735d28635db80
};

#endif
