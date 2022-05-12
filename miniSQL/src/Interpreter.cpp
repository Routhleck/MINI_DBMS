#include "Interpreter.h"
#include <stdexcept>
#include <cstring>

using namespace std;

Interpreter::Interpreter(){
	//创建interpreter
}

Interpreter::~Interpreter(){
	//删除interpreter
}

int Interpreter::interpret(string s){
    int tmp = 0;
	string command;

	//给command赋值第一个指定(读到空格停止)
	command = getCommand(s, tmp);

	//创建命令
	if(command == "create"){
		//给command赋值第二个指令
        command = getCommand(s, tmp);

		//创建表
		if(command == "table"){
			//初始化表名和主键
			string primaryKey = "";
			string tableName = "";

			//给command赋值表名
			command = getCommand(s, tmp);
			if(!command.empty())	
				//创建表名
				tableName = command;
			else{
				//异常处理:没有输入表名
				cout << "Syntax Error! No table name" << endl;
				return 0;
			}
			//读取完表名后,继续读取一位给command赋值
			command = getCommand(s, tmp);
			if(command.empty() || command != "("){
				//异常处理:若无左括号,语法错误
				cout << "Syntax Error! Miss '('" << endl;
				return 0;
			}
			else{
				//读到括号,继续读取
				//此时读到的为第一个变量名
				command = getCommand(s, tmp);
				//创建attribute向量
				vector<Attribute> attributeVector;
				//进行循环读取
				while (!command.empty() && command != "primary"
                        && command != ")"){

					//初始化 attributeName,type,ifUnique
					string attributeName = command;
					int type = 0;
					bool ifUnique = false;

					//处理数据类型
					//读取一位赋值给command
					//int 0;float -1; char >0;
					command = getCommand(s, tmp);
					if(command == "int"){
						type = 0;
                    }
					else if(command == "float"){
						type = -1;
                    }
					else if(command == "char"){
						command = getCommand(s, tmp);
						//char后应跟上'('表示char的大小
						//异常处理:没有左括号
						if (command != "("){
							cout << "Syntax Error: unknown data type" << endl;
							return 0;
						}
						command = getCommand(s, tmp);
						istringstream convert(command);
						//type = covert
						if (!(convert >> type)){
							cout << "Syntax error : illegal number in char()" << endl;
							return 0;
						}
						//异常处理:没有有括号
						command = getCommand(s, tmp);
						if (command != ")"){
							cout << "Syntax Error: unknown data type" << endl;
							return 0;
						}
					}
					else{
						//暂时只支持int,float,char三种数据类型
						cout << "Syntax Error! Unknown data type" << endl;
						return 0;
					}
					//继续读取
					command = getCommand(s, tmp);
					//若读到unique关键字
					//ifUnique为正
					if(command == "unique"){
						ifUnique = true;
                        command = getCommand(s, tmp);
					}
					//B+树读入
					Attribute attribute(attributeName,type,ifUnique);
					attributeVector.push_back(attribute);
					if(command != ","){
						if(command != ")"){
							cout << "Syntax Error!" << endl;
							return 0;
						}
						else break;
					}
					//循环读入
					command = getCommand(s, tmp);
				}

				int primaryKeyLocation = 0;
				//若读到primary
				if(command == "primary"){
					command = getCommand(s, tmp);
					//异常处理:primary后没有key
					if(command != "key"){
						cout << "Error in syntax!" << endl;
						return 0;
					}
					//设置主键
					else{
						command = getCommand(s, tmp);
						if(command == "("){
							command = getCommand(s, tmp);
							primaryKey = command;
							int i = 0;
							//查找主键名称设置unique
							for( i = 0; i < attributeVector.size(); i++){
								if(primaryKey == attributeVector[i].getName()){
									attributeVector[i].setUnique(true);
									break;
								}

							}
							//异常处理:没有查询到主键
							if(i == attributeVector.size()){
								cout << "Syntax Error! primary key does not exist" << endl;
								return 0;
							}
							primaryKeyLocation = i;
							command = getCommand(s, tmp);
							//异常处理:没有')'
							if(command != ")"){
								cout << "Syntax Error!" << endl;
								return 0;
							}
						}
						//异常处理:语法错误
						else{
							cout << "Syntax Error!" << endl;
							return 0;
						}
						command = getCommand(s, tmp);
						//异常处理:没有右括号
						if(command != ")"){
							cout << "Syntax Error" << endl;
							return 0;
						}
					}
				}
				//异常处理:没有右括号
				else if(command.empty()){
					cout << "Syntax Error!" << endl;
					return 0;
				}
				//接入API创建表
				api->createTable(tableName, &attributeVector, primaryKey, primaryKeyLocation);
				return 1;
			}
		}
		//创建索引
		else if(command == "index")
		{
			//初始化indexName,tableName,attributeName
			string indexName = "";
			string tableName = "";
			string attributeName = "";

			command = getCommand(s, tmp);
			if(!command.empty()){			
				//索引名赋值
				indexName = command;
            }
			else{
				//异常处理:无索引名
				cout << "Syntax Error!" << endl;
				return 0;
			}

			command = getCommand(s, tmp);
			try {
				//异常处理:语法错误
				logic_error ex("Syntax Error!");
				if(command != "on")
					throw exception(ex);
				command = getCommand(s, tmp);
				if(command.empty())
					throw exception(ex);
				//索引表名
				tableName = command;
				command = getCommand(s, tmp);
				//异常处理:左括号缺失
				if(command != "(")
					throw exception(ex);
				command = getCommand(s, tmp);
				//异常处理:空值
				if(command.empty())
					throw exception(ex);
				//属性名赋值
				attributeName = command;
				command = getCommand(s, tmp);
				//异常处理:无右括号
				if(command != ")")
					throw exception(ex);
				//接入API,创建索引
				api->createIndex(indexName, tableName, attributeName);
				return 1;
			} 
        catch(exception&) {
				//异常处理:语法错误
				cout << "Syntax Error!" << endl;
				return 0;
			}
		}
		//创建数据库
		else if(command == "database"){
			string databaseName = "";
			command = getCommand(s,tmp);

			if (!command.empty())
				//创建表名
				databaseName = command;
			else {
				//异常处理:没有输入数据库名
				cout << "Syntax Error! No database name" << endl;
				return 0;
			}
			//调用API创建数据库
			api->createDatabase(databaseName);
		}
		else{
			//异常处理:语法错误
			cout << "Syntax Error!" << endl;
			return 0;
		}
		return 0;
	}

	//选择命令
	else if(command == "select"){
		//初始化string向量和表名
		vector<string> attrSelected;
		string tableName = "";
		command = getCommand(s, tmp);

		//是否选择全部
		if(command != "*"){
			while(command != "from"){
				attrSelected.push_back(command);
				command = getCommand(s, tmp);
			}
		}
		else{
			command = getCommand(s, tmp);
		}
		//异常处理:没有from语法错误
		if(command != "from"){
			cout << "Syntax Error!" << endl;
			return 0;
		}
		
		command = getCommand(s, tmp);
		//读取完成,表名赋值
		if(!command.empty())
			tableName = command;
		else{
			cout << "Syntax Error!" << endl;
			return 0;
		}

		
		command = getCommand(s, tmp);
		//调用API,选择表中的列
		if(command.empty()){
			if(attrSelected.size() == 0){
				api-> showRecord(tableName);
			}
			else{
				api-> showRecord(tableName, &attrSelected);
			}
			return 1;
		}
		//Where关键字处理
		else if(command == "where"){
			//初始化attributeName,value,operate为equal情况
			string attributeName = "";
			string value = "";
			int operate = Condition::OPERATOR_EQUAL;
			//初始化condition向量
			std::vector<Condition> conditionVector;
			command = getCommand(s, tmp);
			//循环读入
			while(1){
				try {
					//异常处理:ex:语法错误, ex2:没有比较运算符, ex3:没有值
					logic_error ex("Syntax Error!");
					logic_error ex2("Syntax Error! No operator!");
					logic_error ex3("Syntax Error! No value!");
					if(command.empty())
						throw exception(ex);
					attributeName = command ;
					command = getCommand(s, tmp);
					//各种比较运算,主要为vector类型数据比较服务
					if(command == "<=")
						operate = Condition::OPERATOR_LESS_EQUAL;
					else if(command == ">=")
						operate = Condition::OPERATOR_MORE_EQUAL;
					else if(command == "<")
						operate = Condition::OPERATOR_LESS;
					else if(command == ">")
						operate = Condition::OPERATOR_MORE;
					else if(command == "=")
						operate = Condition::OPERATOR_EQUAL;
					else if(command == "<>")
						operate = Condition::OPERATOR_NOT_EQUAL;
					else
						throw exception(ex2);
					command = getCommand(s, tmp);
					//异常处理:没有值
					if(command.empty())
						throw exception(ex3);
					value = command;
					//Condition c比较
					Condition c(attributeName, value, operate);
					conditionVector.push_back(c);
					command = getCommand(s, tmp);
					//读取完成,break
					if(command.empty())
						break;
					//and 关键字,则继续读取
					if(command != "and")
						throw exception(ex);
					command = getCommand(s, tmp);
				} 	
				catch (exception& e) {
					cout << e.what() << endl;
					return 0;
				}
			}
			//调用API,列出符合情况的数据
			if(attrSelected.size()==0)
				api-> showRecord(tableName, NULL, &conditionVector);
			else
				api->showRecord(tableName, &attrSelected, &conditionVector);
			
			return 1;
		}
	}

	//DROP
	else if(command == "drop"){
		command = getCommand(s, tmp);

		//删除表
		if(command == "table"){
			command = getCommand(s, tmp);
			if(!command.empty()){
				api->dropTable(command);
				return 1;
			}
			else{
				cout << "Syntax Error!" << endl;
				return 1;
			}
		}
		//删除索引
		else if(command == "index"){
			command = getCommand(s, tmp);
			if(!command.empty()){
				api->dropIndex(command);
				return 1;
			}
			else{
				cout << "Syntax Error!" << endl;
				return 1;
			}
		}
		//删除数据库
		else if (command == "database") {
			command = getCommand(s, tmp);
			if (!command.empty()) {
				api->dropDatabase(command);
				return 1;
			}
			else {
				cout << "Syntax Error!" << endl;
				return 1;
			}
		}
		else{
			cout << "Syntax Error!" << endl;
			return 0;
		}
	}

	//DELETE
	else if(command == "delete"){
		string tableName = "";
		command = getCommand(s, tmp);
		if(command != "from"){
			cout << "Syntax Error!" << endl;
			return 0;
		}

		command = getCommand(s, tmp);
		if(!command.empty())
			tableName = command;
		else{
			cout << "Syntax Error!" << endl;
			return 0;
		}

		// condition extricate
		command = getCommand(s, tmp);
		if(command.empty()){
			api->deleteRecord(tableName);
			return 1;
		}
		else if(command == "where"){
			string attributeName = "";
			string value = "";
			int operate = Condition::OPERATOR_EQUAL;
			std::vector<Condition> conditionVector;
			command = getCommand(s, tmp);		//col1
			while(1){
				try {
					logic_error ex("Syntax Error!");
					if(command.empty())
						throw exception(ex);
					attributeName = command ;
					command = getCommand(s, tmp);
					if(command == "<=")
						operate = Condition::OPERATOR_LESS_EQUAL;
					else if(command == ">=")
						operate = Condition::OPERATOR_MORE_EQUAL;
					else if(command == "<")
						operate = Condition::OPERATOR_LESS;
					else if(command == ">")
						operate = Condition::OPERATOR_MORE;
					else if(command == "=")
						operate = Condition::OPERATOR_EQUAL;
					else if(command == "<>")
						operate = Condition::OPERATOR_NOT_EQUAL;
					else
						throw exception(ex);
					command = getCommand(s, tmp);
					if(command.empty()) // no condition
						throw exception(ex);
					value = command;
					command = getCommand(s, tmp);
					Condition c(attributeName, value, operate);
					conditionVector.push_back(c);
					if(command.empty()) // no condition
						break;
					if(command != "and")
						throw  exception(ex);;
					command = getCommand(s, tmp);
				} 	catch (exception&) {
					cout << "Syntax Error!" << endl;
					return 0;
				}
			}
			api->deleteRecord(tableName, &conditionVector);
			return 1;
		}
	}

	//INSERT
	else if(command == "insert"){
		string tableName = "";
		std::vector<string> valueVector;
		command = getCommand(s, tmp);
		try {
			logic_error ex("Syntax Error!");
			if(strcmp(command.c_str(),"into") != 0)
				throw  exception(ex);
			command = getCommand(s, tmp);
			if(command.empty())
				throw  exception(ex);
			tableName = command;
			command = getCommand(s, tmp);
			if(command != "values")
				throw  exception(ex);
			command = getCommand(s, tmp);
			if(command != "(")
				throw  exception(ex);
			command = getCommand(s, tmp);
			while (!command.empty() && command != ")"){
				valueVector.push_back(command);
				command = getCommand(s, tmp);
				if(command == ",")  // bug here
					command = getCommand(s, tmp);
			}
			if(command != ")")
				throw  exception(ex);
		} catch (exception&){
			cout << "Syntax Error!" << endl;
			return 0;
		}
		api->insertRecord(tableName, &valueVector);
		return 1;
	}

	//使用数据库
	else if (command == "use") {
		command = getCommand(s, tmp);
		if (!command.empty()) {
			//调用API使用数据库
			api->useDatabase(command);
			return 1;
		}
		else {
			cout << "Syntax Error!" << endl;
			return 1;
		}

	}

	//退出
	else if(command == "quit" || command == "exit"){ 
		return -1;
	}

	//HELP帮助
	else if (command == "help") {
		return -2;
	}

	//运行程序
	else if(command == "execfile"){
		fileName = getCommand(s, tmp);
		cout << "try to open file " << fileName << "." << endl;
		return 2;
	}

	else{
		if(command != "")
			cout << "Error, command " << command << " not found" << endl;
		return 0;
	}
	return 0;
}

string Interpreter::getCommand(string s, int &tmp){
    string command;
    int index1, index2;

	//获取单词的第一个下标
	while ((s[tmp] == ' ' || s[tmp] == '\n'  || s[tmp] == '\t') && s[tmp] != 0)
		tmp++;
	index1 = tmp;

	//获取下一个单词的第一个下标
	if(s[tmp] == '(' || s[tmp] == ',' || s[tmp] == ')'){
		tmp++;
		index2 = tmp;
		command = s.substr(index1, index2 - index1);
	}

	else if(s[tmp] == '\''){
		tmp++;
		while (s[tmp] != '\'' && s[tmp] !=0)
			tmp++;

		if(s[tmp] == '\''){
			index1++;
			index2 = tmp;
			tmp++;
			command = s.substr(index1, index2 - index1);
		}
		else
			command = "";
	}

	else{
		while (s[tmp] != ' ' &&s[tmp] != '(' && s[tmp] != '\n' 
                && s[tmp] != 0 && s[tmp] != ')' && s[tmp] != ',')
			tmp++;

		index2 = tmp;
		if(index1 != index2)
			command = s.substr(index1, index2 - index1);

        else
			command = "";
	}

    return command;
}

string Interpreter::getFilename() {
	return fileName;
}

void Interpreter::setAPI(API* apiInput) {
	api = apiInput;
}
