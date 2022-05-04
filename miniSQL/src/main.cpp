#include <fstream>
#include <iostream>
#include <ctime>

#include "Interpreter.h"
#include "CatalogManager.h"
#include "RecordManager.h"	
#include "IndexManager.h"
#include "API.h"

using namespace std;

clock_t start; //计算一次操作的时间

int main(){
    //打开整个索引文件
	FILE *fp;
	fp = fopen("INDEX_FILE", "r");
	if (fp == NULL) {
		fp = fopen("INDEX_FILE", "a+");
	}
	fclose(fp);
    
    API api;
    CatalogManager cm;
    RecordManager rm;
    
    //连接API和其他模块
    api.setRecordManager(&rm);
    api.setCatalogManager(&cm);
    IndexManager im(&api);
    api.setIndexManager(&im);
    rm.setAPI(&api);
    int fileExec = 0;
    ifstream file;
    Interpreter interpreter;
    interpreter.setAPI(&api);
    string s;
    int status = 0;

	start = 0; //开始计时

	srand(time(NULL));

    ////打印minisql的基本信息
    cout << "\nCopyright 2018, Made by ORACLE就是寄吧. All rights reserved." << endl;
	cout << "\nType 'help' for help." << endl;

    while(true){
        if(fileExec){
            //执行sql文件
            file.open(interpreter.getFilename());
            if(!file.is_open()){
                cout<<"Fail to open "<<interpreter.getFilename()<<endl;
                fileExec = 0;
                continue;
            }
            while(getline(file,s,';')){
                interpreter.interpret(s);
            }
            file.close();
            fileExec = 0;
        }
        else{
            cout<<"\nminisql>";
            getline(cin,s,';');
			start = clock();
            status =  interpreter.interpret(s);
            if(status==2){
                //执行文件
                fileExec = 1;
            }
			else if (status == -2) {
                //帮助信息
				cout << "仅支持 int \ float \ char(n)." << endl;
				cout << "可以使用create, drop, delete, insert, select命令." << endl;
				cout << "输入 'quit' 或 'exit'来退出." << endl;
			}
            else if(status==-1){
                //退出SQL
                break;
            }
        }
    }

    return 0;
}
