#include "CatalogManager.h"

using namespace std;

#define UNKNOWN_FILE -10
#define TABLE_FILE -20
#define INDEX_FILE -30
#define DATABASE_FILE -40

string useDatabase = "";

CatalogManager::CatalogManager() {
	// TODO Auto-generated constructor stub

}

CatalogManager::~CatalogManager() {
	// TODO Auto-generated destructor stub
}

//删除表,成功->返回1,失败->返回0
int CatalogManager::dropTable(string tableName) {
	string tableFileName = "表目录" + tableName;
	//调用bufferManager删除指定表文件
	bm.deleteFileNode(tableFileName.c_str());
	if (remove(tableFileName.c_str()))
		return 0;
	return 1;
}

//返回索引位置,若没有找到则返回-2
int CatalogManager::getIndexType(string indexName) {
	//初始化fileNode 和blockNode
	fileNode *ftmp = bm.getFile("文件索引");
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		char* addressBegin;
		//调用bufferManager获取地址内容
		addressBegin = bm.getContent(*btmp);
		IndexInfo * i = (IndexInfo *)addressBegin;
		//循环检索索引位置
		for (int j = 0; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo)); j++) {
			if ((*i).getIndexName() == indexName) {
				return i->getType();
			}
			i++;
		}
		return -2;
	}
	return -2;
}

//返回所有索引,用向量存储
int CatalogManager::getAllIndex(vector<IndexInfo> * indexs) {
	//初始化fileNode和blockNode
	fileNode *ftmp = bm.getFile("文件索引");
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		char* addressBegin;
		addressBegin = bm.getContent(*btmp);
		IndexInfo * i = (IndexInfo *)addressBegin;
		//循环存储索引
		for (int j = 0; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo)); j++) {
			indexs->push_back((*i));
			i++;
		}
	}
	return 1;
}

//添加索引,若成功添加->返回,若失败->返回0
int CatalogManager::addIndex(string indexName, string tableName, string Attribute, int type) {
	//初始化fileNode和blockNode
	fileNode *ftmp = bm.getFile("文件索引");
	blockNode *btmp = bm.getBlockHead(ftmp);
	IndexInfo i(indexName, tableName, Attribute, type);
	while (true) {
		//容错:btmp为空
		if (btmp == NULL) {
			return 0;
		}
		//若bufferManager的块已使用大小≤block块可使用大小 - 添加索引的大小
		//可以理解为此时块还有没有空间,可以添加索引
		if (bm.getUsingSize(*btmp) <= bm.getBlockSize() - sizeof(IndexInfo)) {
			char* addressBegin;
			//内存地址起始 = 已使用地址内容 + 块可用的地址
			addressBegin = bm.getContent(*btmp) + bm.getUsingSize(*btmp);
			//将索引大小复制到addressBegin
			memcpy(addressBegin, &i, sizeof(IndexInfo));
			//更改已使用大小(+添加索引大小)
			bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) + sizeof(IndexInfo));
			//更改为脏块
			bm.setDirty(*btmp);
			//设置索引
			return this->setIndexOnAttribute(tableName, Attribute, indexName);
		}
		else {
			//获取下一个块,直至可用
			btmp = bm.getNextBlock(ftmp, btmp);
		}
	}
	return 0;
}

//查找表,若找到->返回TABLE_FILE,失败->返回0
int CatalogManager::findTable(string tableName) {
	FILE *fp;
	string tableFileName = "表目录" + tableName;
	//只读打开表目录
	fp = fopen(tableFileName.c_str(), "r");
	if (fp == NULL) {
		return 0;
	}
	else {
		fclose(fp);
		return TABLE_FILE;
	}
}

//查找数据库文件夹是否存在
int CatalogManager::findDatabase(string databaseName)
{
	//查看文件夹是否存在
	if (access(databaseName.c_str(), 0) == -1)
		return 0;
	else
		return DATABASE_FILE;
	return 0;
}

//查找索引,若找到->返回INDEX_FILE,没找到->返回UNKNOWN_FILE,此块不存在->返回0
int CatalogManager::findIndex(string fileName) {
	//初始化ftmp和btmp
	fileNode *ftmp = bm.getFile("文件索引");
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		char* addressBegin;
		addressBegin = bm.getContent(*btmp);
		IndexInfo * i = (IndexInfo *)addressBegin;
		//先将flag设置为UNKNOWN_FILE
		int flag = UNKNOWN_FILE;
		//循环查找索引,若找到则flag为INDEX_FILE
		for (int j = 0; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo)); j++) {
			if ((*i).getIndexName() == fileName) {
				flag = INDEX_FILE;
				break;
			}
			i++;
		}
		return flag;
	}
	return 0;
}

//删除索引,成功->返回1.失败->返回0
int CatalogManager::dropIndex(string index) {
	//初始化ftmp和btmp
	fileNode *ftmp = bm.getFile("文件索引");
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		char* addressBegin;
		addressBegin = bm.getContent(*btmp);
		IndexInfo * i = (IndexInfo *)addressBegin;
		int j = 0;
		//循环查找索引
		for (j = 0; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo)); j++) {
			if ((*i).getIndexName() == index)
				break;
			i++;
		}
		//删除索引
		this->revokeIndexOnAttribute((*i).getTableName(), (*i).getAttribute(), (*i).getIndexName());
		//将后续索引全部变为下一个索引(删除的索引去除掉)
		for (; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo) - 1); j++) {
			(*i) = *(i + sizeof(IndexInfo));
			i++;
		}
		//调用bufferManager减少使用大小
		//设置为脏块
		bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) - sizeof(IndexInfo));
		bm.setDirty(*btmp);

		return 1;
	}
	return 0;
}

//删除索引具体实现,为上一个dropIndex服务,若成功->返回1,失败->返回0
int CatalogManager::revokeIndexOnAttribute(string tableName, string AttributeName, string indexName) {
	//初始化tableFileName,fileNode,blockNode
	string tableFileName = "表目录" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		//addressBegin = 内容地址 + 1+int大小 + 1
		char* addressBegin = bm.getContent(*btmp);
		addressBegin += (1 + sizeof(int));
		int size = *addressBegin;
		addressBegin++;
		Attribute *a = (Attribute *)addressBegin;
		int i;
		//循环查找地址
		for (i = 0; i<size; i++) {
			//属性名是否匹配
			if (a->getName() == AttributeName) {
				//索引名是否匹配
				if (a->getIndex() == indexName) {
					//索引名设为空
					a->getIndex() = "";
					//调用bufferManager将此块设为脏
					bm.setDirty(*btmp);
				}
				else {
					//异常处理:撤销位置索引
					cout << "撤销未知索引: " << indexName << " on " << tableName << "!" << endl;
					cout << "属性: " << AttributeName << " on 表 " << tableName << " 有索引: " << a->getIndex() << "!" << endl;
				}
				break;
			}
			a++;
		}
		if (i<size)
			return 1;
		else
			return 0;
	}
	return 0;
}

//获取索引名列表,成功->返回1,失败->返回0
int CatalogManager::getIndexNameList(string tableName, vector<string>* indexNameVector) {
	//初始化ftmp和btmp
	fileNode *ftmp = bm.getFile("文件索引");
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		char* addressBegin;
		addressBegin = bm.getContent(*btmp);
		IndexInfo * i = (IndexInfo *)addressBegin;
		//循环获取,存入string向量
		for (int j = 0; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo)); j++) {
			if ((*i).getTableName() == tableName) {
				(*indexNameVector).push_back((*i).getIndexName());
			}
			i++;
		}
		return 1;
	}
	return 0;
}

//删除数据,成功->返回 *recordNum,失败->返回0
int CatalogManager::deleteValue(string tableName, int deleteNum) {
	//初始化tableFileName,fileNode,blockNode
	string tableFileName = "表目录" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		char* addressBegin = bm.getContent(*btmp);
		int * recordNum = (int*)addressBegin;
		//存储数据量需＞删除数量
		if ((*recordNum) <deleteNum) {
			cout << "CatalogManager出现错误::删除数据" << endl;
			return 0;
		}
		else
			//删除此块需要删除数据量
			(*recordNum) -= deleteNum;
		//设置脏快
		bm.setDirty(*btmp);
		return *recordNum;
	}
	return 0;
}

//插入数据,成功->返回originalRecordNum,失败->返回0
int CatalogManager::insertRecord(string tableName, int recordNum) {
	//初始化tableFileName,fileNode,blockNode
	string tableFileName = "表目录" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		char* addressBegin = bm.getContent(*btmp);
		int * originalRecordNum = (int*)addressBegin;
		//返回originalRecordNum增加本身的存储的大小
		*originalRecordNum += recordNum;
		bm.setDirty(*btmp);
		return *originalRecordNum;
	}
	return 0;
}

//获取数据的数量,成功->返回recordNum,失败->返回0
int CatalogManager::getRecordNum(string tableName) {
	string tableFileName = "表目录" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);

	if (btmp) {
		char* addressBegin = bm.getContent(*btmp);
		int * recordNum = (int*)addressBegin;
		return *recordNum;
	}
	return 0;
}

//添加表,成功->返回1,失败->返回0
int CatalogManager::addTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName = "", int primaryKeyLocation = 0) {
	//初始化fp,tableFileName
	FILE *fp;
	string tableFileName = "表目录" + tableName;
	//写入文件
	fp = fopen(tableFileName.c_str(), "w+");
	//容错:fp为空
	if (fp == NULL)
		return 0;
	fclose(fp);
	//初始化ftmp,btmp
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		char* addressBegin = bm.getContent(*btmp);
		int * size = (int*)addressBegin;
		//addressBegin空出一个int的大小
		*size = 0;
		addressBegin += sizeof(int);
		//主键占用内存
		*addressBegin = primaryKeyLocation;
		addressBegin++;
		//属性占用内存
		*addressBegin = (*attributeVector).size();
		addressBegin++;
		//memcpy(addressBegin, attributeVector, (*attributeVector).size()*sizeof(Attribute));
		//循环增加内存地址
		for (int i = 0; i<(*attributeVector).size(); i++) {
			memcpy(addressBegin, &((*attributeVector)[i]), sizeof(Attribute));
			addressBegin += sizeof(Attribute);
		}
		//更改可用空间,设为脏块
		bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) + (*attributeVector).size() * sizeof(Attribute) + 2 + sizeof(int));
		bm.setDirty(*btmp);
		return 1;
	}
	return 0;
}
//添加数据库,成功->返回1,失败->返回0
int CatalogManager::addDatabase(string databaseName) {
	useDatabase = databaseName;
	return 1;
}

//添加索引具体实现,为addIndex服务,成功->返回1,失败->返回0
int CatalogManager::setIndexOnAttribute(string tableName, string AttributeName, string indexName) {
	//初始化tableFileName,fileNode,blockNode
	string tableFileName = "表目录" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		//计算开始地址
		char* addressBegin = bm.getContent(*btmp);
		addressBegin += 1 + sizeof(int);
		int size = *addressBegin;
		addressBegin++;
		Attribute *a = (Attribute *)addressBegin;
		int i;
		//循环查找索引名
		for (i = 0; i<size; i++) {
			//判断属性名
			if (a->getName() == AttributeName) {
				a->setIndex(indexName);
				//设置为脏块
				bm.setDirty(*btmp);
				break;
			}
			a++;
		}
		if (i<size)
			return 1;
		else
			return 0;
	}
	return 0;
}

//获取属性,将属性添加至attributeVector,成功->返回1,失败->返回0
int CatalogManager::getAttribute(string tableName, vector<Attribute>* attributeVector) {
	//初始化tableFileName,fileNode,blockNode
	string tableFileName = "表目录" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		char* addressBegin = bm.getContent(*btmp);
		addressBegin += 1 + sizeof(int);
		int size = *addressBegin;
		addressBegin++;
		Attribute *a = (Attribute *)addressBegin;
		for (int i = 0; i<size; i++) {
			//将属性添加至属性向量
			attributeVector->push_back(*a);
			a++;
		}

		return 1;
	}
	return 0;
}

//计算length,成功->返回singleRecordSize,失败->返回0
int CatalogManager::calcuteLenth(string tableName) {
	//初始化tableFileName,fileNode,blockNode
	string tableFileName = "表目录" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//容错:btmp为空
	if (btmp) {
		int singleRecordSize = 0;
		char* addressBegin = bm.getContent(*btmp);
		addressBegin += 1 + sizeof(int);
		int size = *addressBegin;
		addressBegin++;
		Attribute *a = (Attribute *)addressBegin;
		for (int i = 0; i<size; i++) {//根据数据类型分别计算长度
			if ((*a).getType() == -1) {
				singleRecordSize += sizeof(float);
			}
			else if ((*a).getType() == 0) {
				singleRecordSize += sizeof(int);
			}
			else if ((*a).getType()>0) {
				singleRecordSize += (*a).getType() * sizeof(char);
			}
			else {
				//容错:
				cout << "目录数据损坏！" << endl;
				return 0;
			}
			a++;
		}
		return singleRecordSize;
	}
	return 0;
}

//计算length(重载输入为int)成功->返回各类型数据长度,失败->返回0
int CatalogManager::calcuteLenth(int type) {
	if (type == Attribute::TYPE_INT) {
		return sizeof(int);
	}
	else if (type == Attribute::TYPE_FLOAT)
		return sizeof(float);
	else
		return (int)type * sizeof(char); // 该类型存储在 Attribute.h
}

// 通过表名和recordContent获取表的记录字符串，并将结果写入recordResult引用。
void CatalogManager::getRecordString(string tableName, vector<string>* recordContent, char* recordResult) {
	//初始化attributeVector,tableFileName,contentBegin
	vector<Attribute> attributeVector;
	string tableFileName = "表目录" + tableName;
	getAttribute(tableName, &attributeVector);
	char * contentBegin = recordResult;

	//循环把所有属性增加到内存空间
	for (int i = 0; i < attributeVector.size(); i++) {
		Attribute attribute = attributeVector[i];
		string content = (*recordContent)[i];
		int type = attribute.getType();
		int typeSize = calcuteLenth(type);
		stringstream ss;
		ss << content;
		if (type == Attribute::TYPE_INT) {
			//内容是 int
			int intTmp;
			ss >> intTmp;
			memcpy(contentBegin, ((char*)&intTmp), typeSize);
		}
		else if (type == Attribute::TYPE_FLOAT) {
			//内容是 float
			float floatTmp;
			ss >> floatTmp;
			memcpy(contentBegin, ((char*)&floatTmp), typeSize);
		}
		else {
			//内容是 string
			memcpy(contentBegin, content.c_str(), typeSize);
		}

		contentBegin += typeSize;
	}
	return;
}
