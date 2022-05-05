#include "API.h"
#include "IndexManager.h"
#include <cstring>

using namespace std;

#define UNKNOWN_FILE -10 //不存在文件
#define TABLE_FILE -20 //包含表信息的文件
#define INDEX_FILE -30 //包含索引信息的文件

CatalogManager *cm;
IndexManager* im;

Condition::Condition() {
    //默认创建Condition
}

Condition::~Condition() {
    //默认删除Condition
}

Condition::Condition(string attributeInput, string valueInput, int operateInput) :
attributeName(attributeInput),value(valueInput),operate(operateInput){
	//带有输入的创建
}
//INT类判断
bool Condition::FitAttribute(int content) {
    //初始化myContent = ss
	stringstream ss;
	ss << value;
	int myContent;
	ss >> myContent;

    //判断执行哪种条件
	switch (operate) {
	case Condition::OPERATOR_EQUAL:
		return content == myContent;
	case Condition::OPERATOR_NOT_EQUAL:
		return content != myContent;
	case Condition::OPERATOR_LESS:
		return content < myContent;
	case Condition::OPERATOR_MORE:
		return content > myContent;
	case Condition::OPERATOR_LESS_EQUAL:
		return content <= myContent;
	case Condition::OPERATOR_MORE_EQUAL:
		return content >= myContent;
	default:
		return true;
	}
}
//FLOAT类判断
bool Condition::FitAttribute(float content) {
	stringstream ss;
	ss << value;
	float myContent;
	ss >> myContent;

	switch (operate) {
	case Condition::OPERATOR_EQUAL:
		return content == myContent;
	case Condition::OPERATOR_NOT_EQUAL:
		return content != myContent;
	case Condition::OPERATOR_LESS:
		return content < myContent;
	case Condition::OPERATOR_MORE:
		return content > myContent;
	case Condition::OPERATOR_LESS_EQUAL:
		return content <= myContent;
	case Condition::OPERATOR_MORE_EQUAL:
		return content >= myContent;
	default:
		return true;
	}
}
//STRING类判断
bool Condition::FitAttribute(string content) {
	string myContent = value;
	switch (operate) {
	case Condition::OPERATOR_EQUAL:
		return content == myContent;
	case Condition::OPERATOR_NOT_EQUAL:
		return content != myContent;
	case Condition::OPERATOR_LESS:
		return content < myContent;
	case Condition::OPERATOR_MORE:
		return content > myContent;
	case Condition::OPERATOR_LESS_EQUAL:
		return content <= myContent;
	case Condition::OPERATOR_MORE_EQUAL:
		return content >= myContent;
	default:
		return true;
	}
}

string Condition::getValue() {
	return value;
}

string Condition::getAttributeName() {
	return attributeName;
}

int Condition::getOperate() {
	return operate;
}


API::API():length(0){
    //默认创建API
}

API::~API(){
    //默认删除API
}

//删除表
void API::dropTable(string tableName){
    //容错:表名不存在
    if (!tableExist(tableName)) 
        return;
    //索引名STRING向量
    vector<string> indexNameVector;

    //获取表中的所有索引，然后删除它们
    getIndexNameList(tableName, &indexNameVector);
    for (int i = 0; i < indexNameVector.size(); i++){
        cout << indexNameVector[i];    
        dropIndex(indexNameVector[i]);
    }
    
    //delete a table file
    if(rm->dropTable(tableName)){
        //delete a table information
        cm->dropTable(tableName);
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "查询完毕, 表 " << tableName << " 已被删除.(" << duration << " s)"<< endl;
    }
}

void API::dropIndex(string indexName){
    if (cm->findIndex(indexName) != INDEX_FILE){
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "没有索引 " << indexName <<".("<< duration <<" s)"<< endl;
        return;
    }
    
    //删除索引文件
    if (rm->dropIndex(indexName)){
        //get type of index
        int indexType = cm->getIndexType(indexName);
        if (indexType == -2){
			clock_t finish = clock();
			double duration = (double)(finish - start) / CLOCKS_PER_SEC;
            cout << "错误!("<<duration <<" s)" << endl;
            return;
        }

        //删除索引信息
        cm->dropIndex(indexName);
        
        //删除索引树
        im->dropIndex(rm->getIndexFileName(indexName), indexType);

		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		cout << "查询完毕, 索引 " << indexName << " 已被删除.(" << duration << " s)" << endl;
    }
}

void API::createIndex(string indexName, string tableName, string attributeName){
    if (cm->findIndex(indexName) == INDEX_FILE){
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "已经存在索引 " << indexName << " .(" <<duration <<" s)"<< endl;
        return;
    }
    
    if (!tableExist(tableName)) 
        return;
    
    vector<Attribute> attributeVector;
    cm->getAttribute(tableName, &attributeVector);
    int i;
    int type = 0;
    for (i = 0; i < attributeVector.size(); i++){
        if (attributeName == attributeVector[i].getName()){
            if (!attributeVector[i].getUnique()){
				clock_t finish = clock();
				double duration = (double)(finish - start) / CLOCKS_PER_SEC;
                cout << "属性不是唯一的.("<<duration<<" s)" << endl;
                return;
            }
            type = attributeVector[i].getType();
            break;
        }
    }
    
    if (i == attributeVector.size()){
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "表中没有这个属性.("<<duration<<" s)" << endl;
        return;
    }
    
     //创建索引文件
    if (rm->createIndex(indexName)){
        //CatalogManager来添加索引信息
        cm->addIndex(indexName, tableName, attributeName, type);
        
        //获取索引类型
        int indexType = cm->getIndexType(indexName);
        if (indexType == -2){
			clock_t finish = clock();
			double duration = (double)(finish - start) / CLOCKS_PER_SEC;
            cout << "错误!("<<duration<<" s)" << endl;
            return;
        }
        
        //创建一个索引树
        im->createIndex(rm->getIndexFileName(indexName), indexType);
        
        //recordManager将已经记录插入索引
        rm->indexRecordAllAlreadyInsert(tableName, indexName);

		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		cout << "查询完毕, 索引 " << indexName << " 已被插入.(" << duration << " s)" << endl;
    }
    else{
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "创建索引 " << indexName << " 失败.("<<duration<<" s)" << endl;
    }
}

void API::createTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName,int primaryKeyLocation){   
    if(cm->findTable(tableName) == TABLE_FILE){
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "已经存在表 " << tableName << " .("<<duration<<" s)" << endl;
        return;
    }
    
    //创建表文件
    if(rm->createTable(tableName)){
        //CatalogManager来创建表信息
        cm->addTable(tableName, attributeVector, primaryKeyName, primaryKeyLocation);
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		cout << "查询完毕, 表 " << tableName << " 已被创建.(" << duration << " s)" << endl;
    }
    
    if (primaryKeyName != ""){
        //获取主键
        string indexName = getPrimaryIndex(tableName);
        createIndex(indexName, tableName, primaryKeyName);
    }
}

void API::showRecord(string tableName, vector<string>* attributeNameVector){
    vector<Condition> conditionVector;
    showRecord(tableName, attributeNameVector, &conditionVector);
}


void API::showRecord(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector){
    if (cm->findTable(tableName) == TABLE_FILE){
        int num = 0;
        vector<Attribute> attributeVector;
        getAttribute(tableName, &attributeVector);
        
        vector<string> allAttributeName;
        if (attributeNameVector == NULL) {
            for (Attribute attribute : attributeVector){
                allAttributeName.insert(allAttributeName.end(), attribute.getName());
            }
            attributeNameVector = &allAttributeName;
        }
        
        for (string name : (*attributeNameVector)){
            int i = 0;
            for (i = 0; i < attributeVector.size(); i++){
                if (attributeVector[i].getName() == name)
                    break;
            }
            if (i == attributeVector.size()){
				clock_t finish = clock();
				double duration = (double)(finish - start) / CLOCKS_PER_SEC;
                cout << "要打印的属性在表中不存在.("<<duration<<" s)" << endl;
                return;
            }
        }
        
        int blockOffset = -1;
        if (conditionVector != NULL){
            for (Condition condition : *conditionVector){
                int i = 0;
                for (i = 0; i < attributeVector.size(); i++){
                    if (attributeVector[i].getName() == condition.getAttributeName()){
                        if (condition.getOperate() == Condition::OPERATOR_EQUAL && attributeVector[i].getIndex() != ""){
                                blockOffset = im->searchIndex(rm->getIndexFileName(attributeVector[i].getIndex()), condition.getValue(), attributeVector[i].getType());
                        }
                        break;
                    }
                }
                
                if (i == attributeVector.size()){
					clock_t finish = clock();
					double duration = (double)(finish - start) / CLOCKS_PER_SEC;
                    cout << "该属性在表中不存在.("<< duration<<" s)"<< endl;
                    return;
                }
            }
        }
		//计算值和属性的最大长度
		length = rm->recordLength(tableName, attributeNameVector, conditionVector, blockOffset);	
		
        //打印您想要显示的所有属性
        tableAttributePrint(attributeNameVector);
    
        if (blockOffset == -1){
            //cout << "if we con't find the block by index,we need to find all block" << endl;
            num = rm->recordAllShow(tableName, attributeNameVector,conditionVector);
        }
        else{
            //按索引查找块，在块中搜索
            num = rm->recordBlockShow(tableName, attributeNameVector, conditionVector, blockOffset);
        }

		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		cout << "查询完毕, "<<num<<"记录被更改. ("<< duration << " s)" << endl;
    }
    else{
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "没有此表 " << tableName <<".("<<duration<<" s)"<< endl;
    }

	length = 0; //prepare for next query
}

void API::insertRecord(string tableName, vector<string>* recordContent){
    if (!tableExist(tableName)) return;
    
    string indexName;
    vector<Attribute> attributeVector; 
    vector<Condition> conditionVector;
    
    getAttribute(tableName, &attributeVector);
    for (int i = 0 ; i < attributeVector.size(); i++){
        indexName = attributeVector[i].getIndex();
        if (indexName != ""){
            //如果属性有索引
            int blockoffest = im->searchIndex(rm->getIndexFileName(indexName), (*recordContent)[i], attributeVector[i].getType());
            if (blockoffest != -1){
                //如果该值已经存在于索引树中，则不能插入记录
				clock_t finish = clock();
				double duration = (double)(finish - start) / CLOCKS_PER_SEC;
                cout << "由于索引值存在，插入失败.("<<duration<<" s)" << endl;
                return;
            }
        }
        else if (attributeVector[i].getUnique()){
            //如果属性是唯一的，但不是索引
            Condition condition(attributeVector[i].getName(), (*recordContent)[i], Condition::OPERATOR_EQUAL);
            conditionVector.insert(conditionVector.end(), condition);
        }
    }
    
    if (conditionVector.size() > 0){
        for (int i = 0; i < conditionVector.size(); i++) {
            vector<Condition> conditionTmp;
            conditionTmp.insert(conditionTmp.begin(), conditionVector[i]);
            int recordConflictNum =  rm->recordAllFind(tableName, &conditionTmp);
            if (recordConflictNum > 0) {
				clock_t finish = clock();
				double duration = (double)(finish - start) / CLOCKS_PER_SEC;
                cout << "插入失败，因为存在唯一值.("<<duration<<" s)" << endl;
                return;
            }
        }
    }
    
    char recordString[2000];
    memset(recordString, 0, 2000);
    
    //CatalogManager来获取记录字符串
    cm->getRecordString(tableName, recordContent, recordString);
    
    //将记录插入文件;然后得到块被插入的位置
    int recordSize = cm->calcuteLenth(tableName);
    int blockOffset = rm->insertRecord(tableName, recordString, recordSize);
    
    if(blockOffset >= 0){
        insertRecordIndex(recordString, recordSize, &attributeVector, blockOffset);
        cm->insertRecord(tableName, 1);
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		cout << "查询完毕, 表 " << tableName << " 已被插入记录.(" << duration << " s)" << endl;
    }
    else{
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "向表 " << tableName << " 插入数据失败.("<<duration<<" s)" << endl;
    }
}

void API::deleteRecord(string tableName){
    vector<Condition> conditionVector;
    deleteRecord(tableName, &conditionVector);
}

void API::deleteRecord(string tableName, vector<Condition>* conditionVector){
    if (!tableExist(tableName)) return;
    
    int num = 0;
    vector<Attribute> attributeVector;
    getAttribute(tableName, &attributeVector);

    int blockOffset = -1;
    if (conditionVector != NULL){
        for (Condition condition : *conditionVector){
            if (condition.getOperate() == Condition::OPERATOR_EQUAL){
                for (Attribute attribute : attributeVector){
                    if (attribute.getIndex() != "" && attribute.getName() == condition.getAttributeName()){
                        blockOffset = im->searchIndex(rm->getIndexFileName(attribute.getIndex()), condition.getValue(), attribute.getType());
                    }
                }
            }
        }
    }
    if (blockOffset == -1){
        //如果我们不能通过索引找到块，我们需要找到所有块
        num = rm->recordAllDelete(tableName, conditionVector);
    }
    else{
        //按索引查找块，在块中搜索
        num = rm->recordBlockDelete(tableName, conditionVector, blockOffset);
    }
    
    //删除表中记录的数量
    cm->deleteValue(tableName, num);
	clock_t finish = clock();
	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "查询完毕, 在表 "<< tableName<<"的数据"<< num <<"已被更改.(" << duration << " s)" << endl;
}

int API::getRecordNum(string tableName){
    if (!tableExist(tableName)) 
        return 0;

    return cm->getRecordNum(tableName);
}

int API::getRecordSize(string tableName){
    if (!tableExist(tableName)) 
        return 0;
    
    return cm->calcuteLenth(tableName);
}

int API::getTypeSize(int type){
    return cm->calcuteLenth(type);
}

//获取表中所有索引名
int API::getIndexNameList(string tableName, vector<string>* indexNameVector){
    if (!tableExist(tableName))
        return 0;

    return cm->getIndexNameList(tableName, indexNameVector);
}

void API::getAllIndexAddressInfo(vector<IndexInfo> *indexNameVector){
    cm->getAllIndex(indexNameVector);
    for (int i = 0; i < (*indexNameVector).size(); i++){
        (*indexNameVector)[i].getIndexName() = rm->getIndexFileName((*indexNameVector)[i].getIndexName());
    }
}

int API::getAttribute(string tableName, vector<Attribute>* attributeVector){
    if (!tableExist(tableName))
        return 0;

    return cm->getAttribute(tableName, attributeVector);
}

void API::insertRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector,  int blockOffset){
    char* contentBegin = recordBegin;
    for (int i = 0; i < (*attributeVector).size() ; i++){
        int type = (*attributeVector)[i].getType();
        int typeSize = getTypeSize(type);
        if ((*attributeVector)[i].getIndex() != ""){
            insertIndex((*attributeVector)[i].getIndex(), contentBegin, type, blockOffset);
        }
        
        contentBegin += typeSize;
    }
}

void API::insertIndex(string indexName, char* contentBegin, int type, int blockOffset){
    string content= "";
    stringstream tmp;
    //如果属性有索引
    if (type == Attribute::TYPE_INT){
        int value = *((int*)contentBegin);
        tmp << value;
    }
    else if (type == Attribute::TYPE_FLOAT){
        float value = *((float* )contentBegin);
        tmp << value;
    }
    else{
        char value[255];
        memset(value, 0, 255);
        memcpy(value, contentBegin, sizeof(type));
        string stringTmp = value;
        tmp << stringTmp;
    }
    tmp >> content;
    im->insertIndex(rm->getIndexFileName(indexName), content, blockOffset, type);
}

void API::deleteRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset){
    char* contentBegin = recordBegin;
    for (int i = 0; i < (*attributeVector).size() ; i++){
        int type = (*attributeVector)[i].getType();
        int typeSize = getTypeSize(type);
        
        string content= "";
        stringstream tmp;
        
        if ((*attributeVector)[i].getIndex() != ""){
            //如果属性有索引
            if (type == Attribute::TYPE_INT){
                int value = *((int*)contentBegin);
                tmp << value;
            }
            else if (type == Attribute::TYPE_FLOAT){
                float value = *((float* )contentBegin);
                tmp << value;
            }
            else{
                char value[255];
                memset(value, 0, 255);
                memcpy(value, contentBegin, sizeof(type));
                string stringTmp = value;
                tmp << stringTmp;
            }
            
            tmp >> content;
            im->deleteIndexByKey(rm->getIndexFileName((*attributeVector)[i].getIndex()), content, type);

        }
        contentBegin += typeSize;
    }

}

int API::tableExist(string tableName){
    if (cm->findTable(tableName) != TABLE_FILE){
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "没有此表 " << tableName<<".("<<duration<<" s)" << endl;
        return 0;
    }
    
    return 1;
}

string API::getPrimaryIndex(string tableName){
    return  "PRIMARY_" + tableName;
}

void API::tableAttributePrint(vector<string>* attributeNameVector){
	int i = 0;
	for (i = 0; i < (*attributeNameVector).size(); i++) {
		if ((*attributeNameVector)[i].size() > length)
			length = (*attributeNameVector)[i].size();
	}
	if (length % 2)
		length = length + 1;

	for (i = 0; i < (*attributeNameVector).size(); i++) {
		for (int j = 0; j < length + 4; j += 2)
			cout << "* ";
	}
	cout << "*\n* ";
    for ( i = 0; i < (*attributeNameVector).size(); i++){
        cout << (*attributeNameVector)[i];
		for (int j = 0; j < length + 2 - (*attributeNameVector)[i].size(); j++)
			cout << " ";
		cout << "* ";
    }
	if (i != 0) {
		cout << "\n";
		for (i = 0; i < (*attributeNameVector).size(); i++) {
			for (int j = 0; j < length + 4; j += 2)
				cout << "* ";
		}
		cout << "*";
	}
	cout << "\n";
}

void API::setRecordManager(RecordManager *rmInput){
    rm = rmInput;
}

void API::setCatalogManager(CatalogManager *cmInput){
    cm = cmInput;
}

void API::setIndexManager(IndexManager *imInput){
    im = imInput;
}

int API::getLength() {
	return length;
}
