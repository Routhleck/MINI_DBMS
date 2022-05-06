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

// 判断类
class Condition {
public:
	Condition();
	~Condition();
	Condition(string attributeInput, string valueInput, int operateInput);
	bool FitAttribute(int content); //INT类判断
	bool FitAttribute(float content); //FLOAT类判断
	bool FitAttribute(string content); //STRING类判断
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


//API类
class API{
public:
    API();
	~API();
    void dropTable(string tableName); //删除表
    void createTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName,int primaryKeyLocation); //创建表
    void dropIndex(string indexName); //删除索引
	void createIndex(string indexName, string tableName, string attributeName); //创建索引
    void showRecord(string tableName, vector<string>* attributeNameVector = NULL); //显示查询记录
	void showRecord(string tableName,  vector<string>* attributeNameVector, vector<Condition>* conditionVector); //显示记录有Where
	void insertRecord(string tableName,vector<string>* recordContent); //插入记录
	void deleteRecord(string tableName); //删除记录
	void deleteRecord(string tableName, vector<Condition>* conditionVector); //删除记录有WHERE
	int getRecordNum(string tableName); //返回记录总数
	int getRecordSize(string tableName); //返回记录大小
	int getTypeSize(int type); //获取属性类型大小
    void getAllIndexAddressInfo(vector<IndexInfo> *indexNameVector); //获取所有索引文件名
    int getAttribute(string tableName, vector<Attribute>* attributeVector); //获取表的属性
    void insertIndex(string indexName, char* value, int type, int blockOffset); //插入索引API
    void deleteRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //删除索引记录
    void insertRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //在索引中插入记录
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
    string getPrimaryIndex(string tableName); //获取主属性的索引
    void tableAttributePrint(vector<string>* name); //打印表的属性
};

#endif
