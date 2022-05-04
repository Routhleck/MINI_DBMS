#ifndef API_H_
#define API_H_

#include <string>
#include <vector>
#include <iostream>

#include "BufferManager.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"

using namespace std;

class Attribute;

// class for judging query conditions like '=' or '>'.
class Condition {
public:
	Condition();
	~Condition();
	Condition(string attributeInput, string valueInput, int operateInput);
	bool FitAttribute(int content); //judge conditions for int
	bool FitAttribute(float content); //judge conditions for float
	bool FitAttribute(string content); //judge conditions for string
	string getValue();
	string getAttributeName();
	int getOperate();

	const static int OPERATOR_EQUAL = 0; //"="
	const static int OPERATOR_NOT_EQUAL = 1; //"<>"
	const static int OPERATOR_LESS = 2; //"<"
	const static int OPERATOR_MORE = 3; //">"
	const static int OPERATOR_LESS_EQUAL = 4; //"<="
	const static int OPERATOR_MORE_EQUAL = 5; //">="

private:
	string attributeName;
	string value; //the value to be compared
	int operate; //the type to be compared
};


//class for API interefere
class API{
public:
    API();
	~API();
    void dropTable(string tableName); //删除表
    void createTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName,int primaryKeyLocation); //创建表
    void dropIndex(string indexName); //删除索引
	void createIndex(string indexName, string tableName, string attributeName); //创建索引
    void showRecord(string tableName, vector<string>* attributeNameVector = NULL); //显示记录
	void showRecord(string tableName,  vector<string>* attributeNameVector, vector<Condition>* conditionVector); //显示记录，有Where
	void insertRecord(string tableName,vector<string>* recordContent); //插入记录
	void deleteRecord(string tableName); //删除记录
	void deleteRecord(string tableName, vector<Condition>* conditionVector); //删除记录，有Where
	int getRecordNum(string tableName); //返回记录数量
	int getRecordSize(string tableName); //返回记录大小
	int getTypeSize(int type); //获取数据类型
    void getAllIndexAddressInfo(vector<IndexInfo> *indexNameVector); //获取所有索引名，赋值与向量中
    int getAttribute(string tableName, vector<Attribute>* attributeVector); //获取表的属性
    void insertIndex(string indexName, char* value, int type, int blockOffset); //插入索引
    void deleteRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //删除记录
    void insertRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //插入记录
    void setRecordManager(RecordManager *rmInput);
    void setCatalogManager(CatalogManager *cmInput);
    void setIndexManager(IndexManager *imInput);
	int getLength();

private:
    RecordManager *rm;
    CatalogManager *cm;
    IndexManager *im;
	int length; //表中最长的属性的长度

    int tableExist(string tableName); //判断表是否存在
    int getIndexNameList(string tableName, vector<string>* indexNameVector); //获取所有索引名
    string getPrimaryIndex(string tableName); //获取主键的索引
    void tableAttributePrint(vector<string>* name); //打印出表中所有的属性
};

#endif
