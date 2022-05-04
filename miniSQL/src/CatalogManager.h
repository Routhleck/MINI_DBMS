#ifndef CATALOGMANAGER_H_
#define CATALOGMANAGER_H_

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "BufferManager.h"
#include "IndexManager.h"

using namespace std;

class CatalogManager {
public:
	BufferManager bm;
	CatalogManager();
	virtual ~CatalogManager();
	int addIndex(string indexName, string tableName, string attributeName, int type); //添加表索引
	int revokeIndexOnAttribute(string tableName, string AttributeName, string indexName); //撤销表索引
	int findTable(string tableName); //找到设计表名的表
	int findIndex(string indexName); //找到设计索引名的索引
	int dropTable(string tableName); //删除一个设计好的表
	int dropIndex(string index); //删除一个设计好的索引
	int deleteValue(string tableName, int deleteNum);// 删除记录个数
	int insertRecord(string tableName, int recordNum);  // 增加记录的数量
	int getRecordNum(string tableName); //获取表的记录数
	int getIndexNameList(string tableName, vector<string>* indexNameVector); //获取表的索引名列表
	int getAllIndex(vector<IndexInfo> * indexs); //获取一个表的所有索引
	int setIndexOnAttribute(string tableName, string AttributeName, string indexName); //在表的属性上设置索引
	int addTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName, int primaryKeyLocation); //添加属性表
	int getIndexType(string indexName);
	int getAttribute(string tableName, vector<Attribute>* attributeVector);
	int calcuteLenth(string tableName); //计算表的长度
	int calcuteLenth(int type); //计算类型的长度
	void getRecordString(string tableName, vector<string>* recordContent, char* recordResult); //在字符串中获取记录
};

#endif
