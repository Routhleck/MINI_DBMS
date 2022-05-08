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
	//添加索引,若成功添加->返回,若失败->返回0
	int addIndex(string indexName, string tableName, string attributeName, int type); 
	//删除索引具体实现,为上一个dropIndex服务,若成功->返回1,失败->返回0
	int revokeIndexOnAttribute(string tableName, string AttributeName, string indexName); 
	//查找表,若找到->返回TABLE_FILE,失败->返回0
	int findTable(string tableName); 
	//查找索引,若找到->返回INDEX_FILE,没找到->返回UNKNOWN_FILE,此块不存在->返回0
	int findIndex(string indexName); 
	//删除表,成功->返回1,失败->返回0
	int dropTable(string tableName); 
	//删除索引,成功->返回1.失败->返回0
	int dropIndex(string index); 
	//删除数据,成功->返回 *recordNum,失败->返回0
	int deleteValue(string tableName, int deleteNum);
	//插入数据,成功->返回originalRecordNum,失败->返回0
	int insertRecord(string tableName, int recordNum); 
	//获取数据的数量,成功->返回recordNum,失败->返回0
	int getRecordNum(string tableName);
	//获取索引名列表,成功->返回1,失败->返回0
	int getIndexNameList(string tableName, vector<string>* indexNameVector); 
	//返回所有索引,用向量存储
	int getAllIndex(vector<IndexInfo> * indexs); 
	//添加索引具体实现,为addIndex服务,成功->返回1,失败->返回0
	int setIndexOnAttribute(string tableName, string AttributeName, string indexName); 
	//添加表,成功->返回1,失败->返回0
	int addTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName, int primaryKeyLocation);
	//返回索引位置,若没有找到则返回-2
	int getIndexType(string indexName);
	//获取属性,将属性添加至attributeVector,成功->返回1,失败->返回0
	int getAttribute(string tableName, vector<Attribute>* attributeVector);
	//计算length,(输入为string)成功->返回singleRecordSize,失败->返回0
	int calcuteLenth(string tableName);
	//计算length(重载输入为int)成功->返回各类型数据长度,失败->返回0
	int calcuteLenth(int type);
	// 通过表名和recordContent获取表的记录字符串，并将结果写入recordResult引用。
	void getRecordString(string tableName, vector<string>* recordContent, char* recordResult);
};

#endif
