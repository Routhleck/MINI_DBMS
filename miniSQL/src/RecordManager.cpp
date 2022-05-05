#include "RecordManager.h"
#include "API.h"
#include <cstring>

using namespace std;

RecordManager::RecordManager() {
    //default ctor
}

RecordManager::~RecordManager() {
    //default dtor
}

int RecordManager::createTable(string tableName){
    string tableFileName = getTableFileName(tableName); //获取要打开的表文件名
    
    FILE *fp;
    fp = fopen(tableFileName.c_str(), "a+");//打开表文件
    if (fp == NULL)
        return 0;
    fclose(fp);//关闭文件
    return 1;
}

int RecordManager::dropTable(string tableName){
    string tableFileName = getTableFileName(tableName);//获取要删除的表文件名
    bm.deleteFileNode(tableFileName.c_str());//调用deleteFileNode

    if (remove(tableFileName.c_str())) //若删除成功
		return 0;
    return 1;
}

int RecordManager::createIndex(string indexName){
    string indexFileName = getIndexFileName(indexName);//获取要创建的索引名
  
    FILE *fp;
    fp = fopen(indexFileName.c_str(), "a+");//打开索引文件
    if (fp == NULL)
        return 0;
    fclose(fp);
    return 1;
}

int RecordManager::dropIndex(string indexName){
    string indexFileName = getIndexFileName(indexName);//获取要删除的索引名
    bm.deleteFileNode(indexFileName.c_str());//调用deleteFileNode
    if (remove(indexFileName.c_str()))//若删除成功
        return 0;
    return 1;
}

int RecordManager::insertRecord(string tableName,char* record, int recordSize){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    while (true){
        if (btmp == NULL)//如果块为空，则返回-1
            return -1;

        if (bm.getUsingSize(*btmp) <= bm.getBlockSize() - recordSize){//块使用的字节大小小于等于可使用的字节大小
            char* addressBegin;
            addressBegin = bm.getContent(*btmp) + bm.getUsingSize(*btmp);//确定起始位置
            memcpy(addressBegin, record, recordSize);//将记录复制到addressBegin
            bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) + recordSize);
            bm.setDirty(*btmp);
            return btmp->offsetNum;
        }
        else
            btmp = bm.getNextBlock(ftmp, btmp);
    }
    
    return -1;
}

int RecordManager::recordAllShow(string tableName, vector<string>* attributeNameVector,  vector<Condition>* conditionVector){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true){
        if (btmp == NULL)//如果块为空，则返回-1
            return -1;

        if (btmp->ifbottom){//若文件节点结束
            int recordBlockNum = recordBlockShow(tableName,attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            return count;//返回记录总数
        }
        else{
            int recordBlockNum = recordBlockShow(tableName, attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);//将块指针指向下一个块
        }
    }
    
    return -1;
}

int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

    if (block == NULL)//如果块为空，则返回-1
        return -1;
    else//块非空
        return  recordBlockShow(tableName, attributeNameVector, conditionVector, block);
}

int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, blockNode* block){
    //如果块为空，则返回-1
    if (block == NULL)
        return -1;
    
    int count = 0;
    
    char* recordBegin = bm.getContent(*block);//获取记录起始点
    vector<Attribute> attributeVector;
    int recordSize = api->getRecordSize(tableName);

    api->getAttribute(tableName, &attributeVector);
    char* blockBegin = bm.getContent(*block);//获取块的起始点
    size_t usingSize = bm.getUsingSize(*block);//块的使用大小
    
	if (usingSize == 0)
		return count;
	cout << "* ";
    while (recordBegin - blockBegin  < usingSize){//若记录起始点减块起始点小于块所需大小
        //如果recordBegin指向一个记录
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector)){
            count ++;//记录数加1
            recordPrint(recordBegin, recordSize, &attributeVector, attributeNameVector);//打印数据
            cout <<"\n* ";
        }
        
        recordBegin += recordSize;//指向下一个记录
    }
	for (int i = 0; i < (*attributeNameVector).size(); i++) {
		for (int j = 0; j < api->getLength() + 4; j += 2)
			cout << "* ";
	}
	cout << "\n";
    
    return count;
}

int RecordManager::recordAllFind(string tableName, vector<Condition>* conditionVector){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;

    while (true){
        if (btmp == NULL)//如果块为空，则返回-1
            return -1;
        if (btmp->ifbottom){//若文件节点结束
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            return count;//返回冲突记录总数
        }
        else{
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);//将块指针指向下一个块
        }
    }
    
    return -1;
}

int RecordManager::recordBlockFind(string tableName, vector<Condition>* conditionVector, blockNode* block){
    //如果块为空，则返回-1
    if (block == NULL)
        return -1;
    int count = 0;
    
    char* recordBegin = bm.getContent(*block);//获取记录起始点
    vector<Attribute> attributeVector;
    int recordSize = api->getRecordSize(tableName);//获取记录大小
    
    api->getAttribute(tableName, &attributeVector);
    
    while (recordBegin - bm.getContent(*block)  < bm.getUsingSize(*block)){
        //如果recordBegin指向一个记录
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
            count++;//记录总数加1
        
        recordBegin += recordSize;//指向下一个记录
    }
    
    return count;
}

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

int RecordManager::recordBlockDelete(string tableName,  vector<Condition>* conditionVector, int blockOffset){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

    if (block == NULL)//如果块为空，则返回-1
        return -1;
    else
        return  recordBlockDelete(tableName, conditionVector, block);
}

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
            memset(recordBegin + i, 0, recordSize);//清空记录
            bm.setUsingSize(*block, bm.getUsingSize(*block) - recordSize);
            bm.setDirty(*block); //表示此块是脏的，稍后需要将其写回磁盘
        }
        else
            recordBegin += recordSize;//指向下一个记录
    }
    
    return count;
}

int RecordManager::indexRecordAllAlreadyInsert(string tableName,string indexName){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true){
        if (btmp == NULL)
            return -1;
        if (btmp->ifbottom){//若文件节点结束
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

int RecordManager::indexRecordBlockAlreadyInsert(string tableName,string indexName,  blockNode* block){
    //如果块为空，则返回-1
    if (block == NULL)
        return -1;
    int count = 0;
    
    char* recordBegin = bm.getContent(*block);//获取记录起始点
    vector<Attribute> attributeVector;
    int recordSize = api->getRecordSize(tableName);//获取记录大小
    
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
        recordBegin += recordSize;//指向下一个记录
    }
    
    return count;
}

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
            if ((*conditionVector)[j].getAttributeName() == attributeName){
                //如果这个属性需要处理条件
                if(!contentConditionFit(content, type, &(*conditionVector)[j])){
                    //如果此记录不符合条件
                    return false;
                }
            }
        }

        contentBegin += typeSize;
    }
    return true;
}

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

void RecordManager::contentPrint(char * content, int type){
    if (type == Attribute::TYPE_INT){
        //如果内容是int类型
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
    else if (type == Attribute::TYPE_FLOAT){
        //如果内容是float类型 
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
    else{
        //如果内容是string类型
        string tmp = content;
		cout << tmp;
		for (int j = 0; j < api->getLength() + 2 - tmp.size(); j++)
			cout << " ";
		cout << "* ";
    }
}

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

string RecordManager::getIndexFileName(string indexName){
    string tmp = "";
    return "INDEX_RECORD_" + indexName;
}

string RecordManager::getTableFileName(string tableName){
    string tmp = "";
    return tmp + "TABLE_RECORD_" + tableName;
}

void RecordManager::setAPI(API* apiInput) {
	api = apiInput;
}


int RecordManager::recordLength(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset) {
	fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
	blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

	//如果块为空，则返回-1
	if (block == NULL)
		return -1;
	int length = 0;

	char* recordBegin = bm.getContent(*block);//获取记录起始点
	vector<Attribute> attributeVector;
	int recordSize = api->getRecordSize(tableName);//获取记录大小

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
				if (type == Attribute::TYPE_INT) {
					//如果内容是int类型
					int tmp = *((int *)content);   //按点获取内容值
					string stmp;
					stringstream ss;
					ss << tmp;
					ss >> stmp;
					if (stmp.size() > length)
					length = stmp.size();
				}
				else if (type == Attribute::TYPE_FLOAT) {
					//如果内容是float类型 
					float tmp = *((float *)content);   //按点获取内容值
					string stmp;
					stringstream ss;
					ss << tmp;
					ss >> stmp;
					if (stmp.size() > length)
						length = stmp.size();
				}
				else {
					//如果内容是string类型 
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
