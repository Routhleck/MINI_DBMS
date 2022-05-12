#ifndef RECORDMANAGER_H_
#define RECORDMANAGER_H_

#include <string>
#include <vector>
#include <iostream>

#include "IndexManager.h"
#include "RecordManager.h"
#include "BufferManager.h"

using namespace std;

class Condition;
class API;

class RecordManager{
public:
	RecordManager();
	~RecordManager();
    
    int createTable(string tableName); // 创建表来初始化记录
    int dropTable(string tableName); //删除一个表和所有记录
    int dropIndex(string indexName); //在属性上删除索引
    int createIndex(string indexName); //在属性上创建索引
	int insertRecord(string tableName, char* record, int recordSize); //向表中插入记录
    int recordAllShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector); //显示所有选择的记录
    int recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset); //显示已设计的记录以供选择
    int recordAllFind(string tableName, vector<Condition>* conditionVector); //查找表的记录
    int recordAllDelete(string tableName, vector<Condition>* conditionVector); //删除一个表的所有记录
    int recordBlockDelete(string tableName,  vector<Condition>* conditionVector, int blockOffset); //删除内存中的记录块
    int indexRecordAllAlreadyInsert(string tableName,string indexName); //为表中已经存在的记录创建索引
    string getTableFileName(string tableName); //返回表信息的文件名
    string getIndexFileName(string indexName); //返回表信息的索引名
	int recordLength(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset); //计算表的最大录长度记
	int recordRowLength(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector); //计算表中一行的最大记录长度
	void setAPI(API* apiInput);

private:
	BufferManager bm;
	API *api;

    int recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, blockNode* block); //显示块中的记录
    int recordBlockFind(string tableName, vector<Condition>* conditionVector, blockNode* block); //找到记录块的位置
    int recordBlockDelete(string tableName,  vector<Condition>* conditionVector, blockNode* block); //从内存中删除一个记录块
    int indexRecordBlockAlreadyInsert(string tableName,string indexName, blockNode* block); //为块创建索引
    bool recordConditionFit(char* recordBegin,int recordSize, vector<Attribute>* attributeVector,vector<Condition>* conditionVector); //符合块的条件
    void recordPrint(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector); //打印记录信息
    bool contentConditionFit(char* content, int type, Condition* condition); // 符合内容的条件
    void contentPrint(char * content, int type); //以其类型打印内容
};

#endif
