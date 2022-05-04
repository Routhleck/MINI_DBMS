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
	int interpret(string s); //将命令解释给API
	string getFilename();
	void setAPI(API* apiInput);

private:
    API* api;
	string fileName;
	string getCommand(string s, int &st); //得到命令中的每个单词
};

#endif
