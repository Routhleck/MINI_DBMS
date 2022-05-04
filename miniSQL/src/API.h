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
	string value; //要比较的值
	int operate; //要比较的类型
};


//API干涉类
class API{
public:
    API();
	~API();
    void dropTable(string tableName); //删除表API干扰
    void createTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName,int primaryKeyLocation); //创建表API干扰
    void dropIndex(string indexName); //删除索引API干扰
	void createIndex(string indexName, string tableName, string attributeName); //创建索引API干扰
    void showRecord(string tableName, vector<string>* attributeNameVector = NULL); //显示查询记录
	void showRecord(string tableName,  vector<string>* attributeNameVector, vector<Condition>* conditionVector); //显示设计状态的记录
	void insertRecord(string tableName,vector<string>* recordContent); //向表中插入记录
	void deleteRecord(string tableName); //删除表中的记录
	void deleteRecord(string tableName, vector<Condition>* conditionVector); //删除设计条件下的记录
	int getRecordNum(string tableName); //返回记录编号
	int getRecordSize(string tableName); //获取记录大小
	int getTypeSize(int type); //获取属性类型大小
    void getAllIndexAddressInfo(vector<IndexInfo> *indexNameVector); //获取所有索引文件名
    int getAttribute(string tableName, vector<Attribute>* attributeVector); //获取表的属性
    void insertIndex(string indexName, char* value, int type, int blockOffset); //插入索引API干扰
    void deleteRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //删除索引中的记录
    void insertRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //在索引中插入记录
    void setRecordManager(RecordManager *rmInput);
    void setCatalogManager(CatalogManager *cmInput);
    void setIndexManager(IndexManager *imInput);
	int getLength();

private:
    RecordManager *rm;
    CatalogManager *cm;
    IndexManager *im;
	int length; //表中最长的值或属性的长度

    int tableExist(string tableName); //判断表是否存在
    int getIndexNameList(string tableName, vector<string>* indexNameVector); //获取所有索引名
    string getPrimaryIndex(string tableName); //获取主属性的索引
    void tableAttributePrint(vector<string>* name); //打印表的属性
};

#endif
