#include "RecordManager.h"
#include "API.h"
#include <cstring>
#include <direct.h>

using namespace std;

RecordManager::RecordManager() {
    //default ctor
}

RecordManager::~RecordManager() {
    //default dtor
}

//创建表
int RecordManager::createTable(string tableName){
    //获取要打开的表文件名
    string tableFileName = getTableFileName(tableName); 
    
    FILE *fp;
    //打开表文件
    fp = fopen(tableFileName.c_str(), "a+");
    if (fp == NULL)
        return 0;
    fclose(fp);//关闭文件
    return 1;
}

//创建数据库
int RecordManager::createDatabase(string databaseName){
    //创建文件夹
    if (_mkdir(databaseName.c_str()) == -1)
        return 0;
    //成功创建返回1
    return 1;
}



//删除表
int RecordManager::dropTable(string tableName){
    //获取要删除的表文件名
    string tableFileName = getTableFileName(tableName);
    //调用deleteFileNode
    bm.deleteFileNode(tableFileName.c_str());

    //若删除成功
    if (remove(tableFileName.c_str())) 
		return 0;
    return 1;
}

//创建索引
int RecordManager::createIndex(string indexName){
    //获取要创建的索引名
    string indexFileName = getIndexFileName(indexName);
  
    FILE *fp;
    //打开索引文件
    fp = fopen(indexFileName.c_str(), "a+");
    if (fp == NULL)
        return 0;
    fclose(fp);
    return 1;
}

//删除索引
int RecordManager::dropIndex(string indexName){
    //获取要删除的索引名
    string indexFileName = getIndexFileName(indexName);
    //调用bufferManager删除索引
    bm.deleteFileNode(indexFileName.c_str());
    //若删除成功
    if (remove(indexFileName.c_str()))
        return 0;
    return 1;
}

//插入记录
int RecordManager::insertRecord(string tableName,char* record, int recordSize){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    while (true){
        //如果块为空，则返回-1
        if (btmp == NULL)
            return -1;

        //块使用的字节大小小于等于可使用的字节大小
        if (bm.getUsingSize(*btmp) <= bm.getBlockSize() - recordSize){
            char* addressBegin;
            //确定起始位置
            addressBegin = bm.getContent(*btmp) + bm.getUsingSize(*btmp);
            //将记录复制到addressBegin
            memcpy(addressBegin, record, recordSize);
            bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) + recordSize);
            bm.setDirty(*btmp);
            return btmp->offsetNum;
        }
        else
            btmp = bm.getNextBlock(ftmp, btmp);
    }
    
    return -1;
}

//显示所有选择的记录
int RecordManager::recordAllShow(string tableName, vector<string>* attributeNameVector,  vector<Condition>* conditionVector){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true){
        //如果块为空，则返回-1
        if (btmp == NULL)
            return -1;

        //若文件节点结束
        if (btmp->ifbottom){
            int recordBlockNum = recordBlockShow(tableName,attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            //返回记录总数
            return count;
        }
        else{
            int recordBlockNum = recordBlockShow(tableName, attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            //将块指针指向下一个块
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

//显示选择的记录有WHERE(重载为blockOffset)
int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

    if (block == NULL)//如果块为空，则返回-1
        return -1;
    else//块非空
        return  recordBlockShow(tableName, attributeNameVector, conditionVector, block);
}

//显示选择的记录有WHERE(重载为blockNode)
int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, blockNode* block){
    //如果块为空，则返回-1
    if (block == NULL)
        return -1;
    
    int count = 0;
    //获取记录起始点
    char* recordBegin = bm.getContent(*block);
    vector<Attribute> attributeVector;
    int recordSize = api->getRecordSize(tableName);

    api->getAttribute(tableName, &attributeVector);
    //获取块的起始点
    char* blockBegin = bm.getContent(*block);
    //块的使用大小
    size_t usingSize = bm.getUsingSize(*block);
    
	if (usingSize == 0)
		return count;
	cout << "* ";
    //若记录起始点减块起始点小于块所需大小
    while (recordBegin - blockBegin  < usingSize){
        //如果recordBegin指向一个记录
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector)){
            count ++;//记录数加1
            //打印记录
            recordPrint(recordBegin, recordSize, &attributeVector, attributeNameVector);
            cout <<"\n* ";
        }

        //指向下一个记录
        recordBegin += recordSize;
    }
	for (int i = 0; i < (*attributeNameVector).size(); i++) {
		for (int j = 0; j < api->getLength() + 4; j += 2)
			cout << "* ";
	}
	cout << "\n";
    
    return count;
}

//显示全部列,有WHERE
int RecordManager::recordAllFind(string tableName, vector<Condition>* conditionVector){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;

    while (true){
        //如果块为空，则返回-1
        if (btmp == NULL)
            return -1;
        //若文件节点结束
        if (btmp->ifbottom){
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            //返回冲突记录总数
            return count;
        }
        else{
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            //将块指针指向下一个块
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}
//找到记录块的位置
int RecordManager::recordBlockFind(string tableName, vector<Condition>* conditionVector, blockNode* block){
    //如果块为空，则返回-1
    if (block == NULL)
        return -1;
    int count = 0;
    //获取记录起始点
    char* recordBegin = bm.getContent(*block);
    vector<Attribute> attributeVector;
    //获取记录大小
    int recordSize = api->getRecordSize(tableName);
    
    api->getAttribute(tableName, &attributeVector);
    
    while (recordBegin - bm.getContent(*block)  < bm.getUsingSize(*block)){
        //如果recordBegin指向一个记录
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
            count++;//记录总数加1
        //指向下一个记录
        recordBegin += recordSize;
    }
    
    return count;
}

//删除一个表的所有记录
int RecordManager::recordAllDelete(string tableName, vector<Condition>* conditionVector){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);

    int count = 0;
    while (true){
        if (btmp == NULL)//如果块为空，则返回-1
            return -1;
        if (btmp->ifbottom){//若文件节点结束
            int recordBlockNum = recordBlockDelete(tableName, conditionVector, btmp);
            count += recordBlockNum;
            return count;//返回删除的记录总数
        }
        else{
            int recordBlockNum = recordBlockDelete(tableName, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);//将块指针指向下一个块
        }
    }
    
    return -1;
}

//从内存中删除一个记录块(重载为blockNum)
int RecordManager::recordBlockDelete(string tableName,  vector<Condition>* conditionVector, int blockOffset){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

    if (block == NULL)//如果块为空，则返回-1
        return -1;
    else
        return  recordBlockDelete(tableName, conditionVector, block);
}

//从内存中删除一个记录块(重载为blockNode)
int RecordManager::recordBlockDelete(string tableName,  vector<Condition>* conditionVector, blockNode* block){
    //如果块为空，则返回-1
    if (block == NULL)
        return -1;
    int count = 0;
    
    char* recordBegin = bm.getContent(*block);//获取记录起始点
    vector<Attribute> attributeVector;
    int recordSize = api->getRecordSize(tableName);//获取记录大小
    
    api->getAttribute(tableName, &attributeVector);
    
    while (recordBegin - bm.getContent(*block) < bm.getUsingSize(*block)){
        //如果recordBegin指向一个记录
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector)){
            count ++;//删除记录数加1
            
            api->deleteRecordIndex(recordBegin, recordSize, &attributeVector, block->offsetNum);//同时删除记录的索引
            int i = 0;
            for (i = 0; i + recordSize + recordBegin - bm.getContent(*block) < bm.getUsingSize(*block); i++){
                recordBegin[i] = recordBegin[i + recordSize];
            }
            //清空记录
            memset(recordBegin + i, 0, recordSize);
            bm.setUsingSize(*block, bm.getUsingSize(*block) - recordSize);
            //表示此块是脏的，稍后需要将其写回磁盘
            bm.setDirty(*block); 
        }
        else
            //指向下一个记录
            recordBegin += recordSize;
    }
    
    return count;
}

//为表中已经存在的记录创建索引
int RecordManager::indexRecordAllAlreadyInsert(string tableName,string indexName){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true){
        if (btmp == NULL)
            return -1;
        //若文件节点结束
        if (btmp->ifbottom){
            int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, btmp);
            count += recordBlockNum;
            return count;
        }
        else{
            int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

//为表中已经存在的记录创建索引(重载有blockNode)
int RecordManager::indexRecordBlockAlreadyInsert(string tableName,string indexName,  blockNode* block){
    //如果块为空，则返回-1
    if (block == NULL)
        return -1;
    int count = 0;
    
    //获取记录起始点
    char* recordBegin = bm.getContent(*block);
    vector<Attribute> attributeVector;
    //获取记录大小
    int recordSize = api->getRecordSize(tableName);
    
    api->getAttribute(tableName, &attributeVector);
    
    int type;
    int typeSize;
    char * contentBegin;
    
    while (recordBegin - bm.getContent(*block)  < bm.getUsingSize(*block)){
        contentBegin = recordBegin;
        //如果recordBegin指向一个记录
        for (int i = 0; i < attributeVector.size(); i++){
            type = attributeVector[i].getType();
            typeSize = api->getTypeSize(type);
            
            //找到记录的索引，并将其插入索引树
            if (attributeVector[i].getIndex()==indexName){
                api->insertIndex(indexName, contentBegin, type, block->offsetNum);
                count++;
            }
            
            contentBegin += typeSize;
        }
        //指向下一个记录
        recordBegin += recordSize;
    }
    
    return count;
}

//符合条件的块
bool RecordManager::recordConditionFit(char* recordBegin,int recordSize, vector<Attribute>* attributeVector,vector<Condition>* conditionVector){
    if (conditionVector == NULL)
        return true;
    
    int type;
    string attributeName;
    int typeSize;
    char content[255];
    
    char *contentBegin = recordBegin;
    for(int i = 0; i < attributeVector->size(); i++){
        type = (*attributeVector)[i].getType();
        attributeName = (*attributeVector)[i].getName();
        typeSize = api->getTypeSize(type);
        
        //初始化内容(当内容是字符串时，我们可以很容易地得到一个字符串)
        memset(content, 0, 255);
        memcpy(content, contentBegin, typeSize);
        for(int j = 0; j < (*conditionVector).size(); j++){
            //如果这个属性需要处理条件
            if ((*conditionVector)[j].getAttributeName() == attributeName){
                //如果此记录不符合条件
                if(!contentConditionFit(content, type, &(*conditionVector)[j])){
                    return false;
                }
            }
        }

        contentBegin += typeSize;
    }
    return true;
}

//打印记录信息
void RecordManager::recordPrint(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector){
    int type;
    string attributeName;
    int typeSize;
    char content[255];
    
    char *contentBegin = recordBegin;
    for(int i = 0; i < attributeVector->size(); i++){
        type = (*attributeVector)[i].getType();
        typeSize = api->getTypeSize(type);
        
        //初始化内容(当内容是字符串时，我们可以很容易地得到一个字符串)
        memset(content, 0, 255);
        memcpy(content, contentBegin, typeSize);

        for(int j = 0; j < (*attributeNameVector).size(); j++){
            if ((*attributeNameVector)[j] == (*attributeVector)[i].getName()){
                contentPrint(content, type);
                break;
            }
        }
        
        contentBegin += typeSize;
    }
}

//以其类型打印内容
void RecordManager::contentPrint(char * content, int type){
    //如果内容是int类型
    if (type == Attribute::TYPE_INT){
        int tmp = *((int *) content);   //按点获取内容值
		cout << tmp;
		stringstream ss;
		ss << tmp;
		string stmp;
		ss >> stmp;
		for (int j = 0; j < api->getLength() + 2 - stmp.size(); j++)
			cout << " ";
		cout << "* ";
    }
    //如果内容是float类型
    else if (type == Attribute::TYPE_FLOAT){
        float tmp = *((float *) content);   //按点获取内容值
		cout << tmp;
		stringstream ss;
		ss << tmp;
		string stmp;
		ss >> stmp;
		for (int j = 0; j < api->getLength() + 2 - stmp.size(); j++)
			cout << " ";
		cout << "* ";
    }
    //如果内容是string类型
    else{
        string tmp = content;
		cout << tmp;
		for (int j = 0; j < api->getLength() + 2 - tmp.size(); j++)
			cout << " ";
		cout << "* ";
    }
}

// 符合内容的条件
bool RecordManager::contentConditionFit(char* content,int type,Condition* condition){
    if (type == Attribute::TYPE_INT){
        //如果内容是int类型
        int tmp = *((int *) content);   //按点获取内容值
        return condition->FitAttribute(tmp);
    }
    else if (type == Attribute::TYPE_FLOAT){
        //如果内容是float类型
        float tmp = *((float *) content);   //按点获取内容值
        return condition->FitAttribute(tmp);
    }
    else{
        //如果内容是string类型
        return condition->FitAttribute(content);
    }
    return true;
}

//返回索引文件名
string RecordManager::getIndexFileName(string indexName){
    string tmp = "";
    return "INDEX_RECORD_" + indexName;
}

//返回表文件名
string RecordManager::getTableFileName(string tableName){
    string tmp = "";
    return tmp + "TABLE_RECORD_" + tableName;
}

//设置API
void RecordManager::setAPI(API* apiInput) {
	api = apiInput;
}

//计算表的最大记录长度
int RecordManager::recordLength(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset) {
	fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
	blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

	//如果块为空，则返回-1
	if (block == NULL)
		return -1;
	int length = 0;
    //获取记录起始点
	char* recordBegin = bm.getContent(*block);
	vector<Attribute> attributeVector;
    //获取记录大小
	int recordSize = api->getRecordSize(tableName);

	api->getAttribute(tableName, &attributeVector);
	char* blockBegin = bm.getContent(*block);
	size_t usingSize = bm.getUsingSize(*block);

	while (recordBegin - blockBegin  < usingSize) {
		//如果recordBegin指向一个记录
		if (recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector)) {
			int lentmp = recordRowLength(recordBegin, recordSize, &attributeVector, attributeNameVector);
			if (lentmp > length)//更新长度
				length = lentmp;
		}
		recordBegin += recordSize;
	}

	return length;
}

//计算表中一行的最大记录长度
int RecordManager::recordRowLength(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector) {
	int type;
	string attributeName;
	int typeSize;
	char content[255];
	int length = 0;

	char *contentBegin = recordBegin;
	for (int i = 0; i < attributeVector->size(); i++) {
		type = (*attributeVector)[i].getType();
		typeSize = api->getTypeSize(type);

		//初始化内容
		memset(content, 0, 255);
		memcpy(content, contentBegin, typeSize);

		for (int j = 0; j < (*attributeNameVector).size(); j++) {
			if ((*attributeNameVector)[j] == (*attributeVector)[i].getName()) {
                //如果内容是int类型
                if (type == Attribute::TYPE_INT) {
					int tmp = *((int *)content);   //按点获取内容值
					string stmp;
					stringstream ss;
					ss << tmp;
					ss >> stmp;
					if (stmp.size() > length)
					length = stmp.size();
				}
                //如果内容是float类型 
				else if (type == Attribute::TYPE_FLOAT) {
					float tmp = *((float *)content);   //按点获取内容值
					string stmp;
					stringstream ss;
					ss << tmp;
					ss >> stmp;
					if (stmp.size() > length)
						length = stmp.size();
				}
                //如果内容是string类型
				else {
					string tmp = content;
					if (tmp.size() > length)
						length = tmp.size();
				}
				break;
			}
		}

		contentBegin += typeSize;
	}
	return length;
}
