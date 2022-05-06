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

    //打开文件, 'r' = 只读
	fp = fopen("INDEX_FILE", "r");
	if (fp == NULL) {
        //打开文件, 'a+' = 输出打开文件，若文件不存在，则创建新文件
		fp = fopen("INDEX_FILE", "a+");
	}
	fclose(fp);
    
    //创建API, CatalogManager， RecordManager实例
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
    cout << "\nCopyright 2022, Made by Group 10. All rights reserved." << endl;
	cout << "\nType 'help' for help." << endl;

    //无限循环
    while(true){
        if(fileExec){

            //执行sql文件
            file.open(interpreter.getFilename());
            if(!file.is_open()){
                cout<<"Fail to open "<<interpreter.getFilename()<<endl;
                fileExec = 0;
                continue;
            }
            //将文件内容按';'拆分
            while(getline(file,s,';')){
                interpreter.interpret(s);
            }
            file.close();
            fileExec = 0;
        }
        else{
            //若不执行sql文件

            cout<<"\nminisql>";
            //按';'读取输入内容
            getline(cin,s,';');

            //计算开始时间
			start = clock();

            //将输入内容输入进interpreter进行解析
            status =  interpreter.interpret(s);

            //status == 2时
            //解析为执行sql
            if(status==2){
                //执行文件
                fileExec = 1;
            }

            //status == -2时
            //解析为帮助信息
			else if (status == -2) {
                //帮助信息
				cout << "仅支持 int \ float \ char(n)." << endl;
				cout << "可以使用create, drop, delete, insert, select命令." << endl;
				cout << "输入 'quit' 或 'exit'来退出." << endl;
			}
            //status == -1时
            //解析为退出sql
            else if(status==-1){
                //退出sql
                break;
            }
        }
    }

    return 0;
}
