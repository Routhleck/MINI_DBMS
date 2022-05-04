#include "BufferManager.h"
#include <cstring>

using namespace std;

BufferManager::BufferManager():totalBlock(0),totalFile(0),fileHead(NULL){
    for (int i = 0; i < MAX_FILE_NUM; i ++){
        file_pool[i].fileName = new char[MAX_FILE_NAME];
        if(file_pool[i].fileName == NULL){
            cout << "在初始化文件池时不能分配内存!" <<endl;
            exit (1);
        }
        initFile(file_pool[i]);
    }
    for (int i = 0; i < MAX_BLOCK_NUM; i ++) {
        block_pool[i].address = new char[BLOCK_SIZE];
        if(block_pool[i].address == NULL){
            cout <<"在初始化块池时不能分配内存!"<<endl;
            exit (1);
        }
        block_pool[i].fileName = new char[MAX_FILE_NAME];
        if(block_pool[i].fileName == NULL){
            cout<<"在初始化块池时不能分配内存!"<<endl;
            exit (1);
        }
        initBlock(block_pool[i]);
    }
}

BufferManager::~BufferManager(){
    writtenBackToDiskAll();
    for (int i = 0; i < MAX_FILE_NUM; i ++){
        delete [] file_pool[i].fileName;
    }
    for (int i = 0; i < MAX_BLOCK_NUM; i ++){
        delete [] block_pool[i].address;
    }
}

void BufferManager::initFile(fileNode &file){
    file.nextFile = NULL;
    file.preFile = NULL;
    file.blockHead = NULL;
    file.pin = false;
    memset(file.fileName,0,MAX_FILE_NAME);
}

void BufferManager::initBlock(blockNode &block){
    memset(block.address,0,BLOCK_SIZE);
    size_t init_usage = 0;
    memcpy(block.address, (char*)&init_usage, sizeof(size_t)); // set the block head
    block.usingSize = sizeof(size_t);
    block.dirty = false;
    block.nextBlock = NULL;
    block.preBlock = NULL;
    block.offsetNum = -1;
    block.pin = false;
    block.reference = false;
    block.ifbottom = false;
    memset(block.fileName,0,MAX_FILE_NAME);
}

fileNode* BufferManager::getFile(const char * fileName, bool if_pin){
    blockNode * btmp = NULL;
    fileNode * ftmp = NULL;
    if(fileHead != NULL){
        for(ftmp = fileHead;ftmp != NULL;ftmp = ftmp->nextFile){
            if(!strcmp(fileName, ftmp->fileName)){ 
				//the fileNode is already in the list
                ftmp->pin = if_pin;
                return ftmp;
            }
        }
    }
    // fileNode不在列表中
    if(totalFile == 0){ 
		// 现在列表中没有文件
        ftmp = &file_pool[totalFile];
        totalFile ++;
        fileHead = ftmp;
    }
    else if(totalFile < MAX_FILE_NUM){ 
		// 文件池中有空的文件节点
        ftmp = &file_pool[totalFile];
        // 将这个fileNode添加到列表的尾部
        file_pool[totalFile-1].nextFile = ftmp;
        ftmp->preFile = &file_pool[totalFile-1];
        totalFile ++;
    }
    else {
		// if totalFile >= MAX_FILE_NUM, find one fileNode to replace, write back the block node belonging to the fileNode
        ftmp = fileHead;
        while(ftmp->pin){
            if(ftmp -> nextFile)ftmp = ftmp->nextFile;
            else {
				//池中没有足够的文件节点
                cout<<"文件池中没有足够的文件节点!";
                exit(2);
            }
        }
        for(btmp = ftmp->blockHead;btmp != NULL;btmp = btmp->nextBlock){
            if(btmp->preBlock){
                initBlock(*(btmp->preBlock));
                totalBlock --;
            }
            writtenBackToDisk(btmp->fileName,btmp);
        }
        initFile(*ftmp);
    }
    if(strlen(fileName) + 1 > MAX_FILE_NAME){
        cout <<"文件名长于最大值 " << MAX_FILE_NAME<<endl;
        exit(3);
    }
    strncpy(ftmp->fileName, fileName,MAX_FILE_NAME);
    setPin(*ftmp, if_pin);
    return ftmp;
}

blockNode* BufferManager::getBlock(fileNode * file,blockNode *position, bool if_pin){
    const char * fileName = file->fileName;
    blockNode * btmp = NULL;
    if(totalBlock == 0){
        btmp = &block_pool[0];
        totalBlock ++;
    }
    else if(totalBlock < MAX_BLOCK_NUM){ // 块池中存在空块
        for(int i = 0 ;i < MAX_BLOCK_NUM;i ++){
            if(block_pool[i].offsetNum == -1){
                btmp = &block_pool[i];
                totalBlock ++;
                break;
            }
            else
                continue;
        }
    }
    else{ // totalBlock >= MAX_BLOCK_NUM,which means that there are no empty block so we must replace one.
        int i = replaced_block;
        while (true){
            i ++;
            if(i >= totalBlock) i = 0;
            if(!block_pool[i].pin){
                if(block_pool[i].reference == true)
                    block_pool[i].reference = false;
                else {
					//选择此块进行更换
                    btmp = &block_pool[i];
                    if(btmp->nextBlock) btmp -> nextBlock -> preBlock = btmp -> preBlock;
                    if(btmp->preBlock) btmp -> preBlock -> nextBlock = btmp -> nextBlock;
                    if(file->blockHead == btmp) file->blockHead = btmp->nextBlock;
                    replaced_block = i; //记录被替换的块，下次从下一个块开始。
                    
                    writtenBackToDisk(btmp->fileName, btmp);
                    initBlock(*btmp);
                    break;
                }
            }
            else // 此块已被锁定
                continue;
        }
    }
    //将块添加到块列表中
    if(position != NULL && position->nextBlock == NULL){
        btmp -> preBlock = position;
        position -> nextBlock = btmp;
        btmp -> offsetNum = position -> offsetNum + 1;
    }
    else if (position !=NULL && position->nextBlock != NULL){
        btmp->preBlock=position;
        btmp->nextBlock=position->nextBlock;
        position->nextBlock->preBlock=btmp;
        position->nextBlock=btmp;
        btmp -> offsetNum = position -> offsetNum + 1;
    }
    else {
		// 块将是列表的头
        btmp -> offsetNum = 0;
        if(file->blockHead){
			// 如果文件有一个错误的块头
            file->blockHead -> preBlock = btmp;
            btmp->nextBlock = file->blockHead;
        }
        file->blockHead = btmp;
    }
    setPin(*btmp, if_pin);
    if(strlen(fileName) + 1 > MAX_FILE_NAME){
        cout << "文件名长于最大值 "<<MAX_FILE_NAME<<endl;
        exit(3);
    }
    strncpy(btmp->fileName, fileName, MAX_FILE_NAME);
    
    //读取文件内容到块
    FILE * fileHandle;
    if((fileHandle = fopen(fileName, "ab+")) != NULL){
        if(fseek(fileHandle, btmp->offsetNum*BLOCK_SIZE, 0) == 0){
            if(fread(btmp->address, 1, BLOCK_SIZE, fileHandle)==0)
                btmp->ifbottom = true;
            btmp ->usingSize = getUsingSize(btmp);
        }
        else{
            cout<<"查找文件 "<<fileName<<" 出现读取问题"<<endl;
            exit(1);
        }
        fclose(fileHandle);
    }
    else{
        cout<<"查找文件 "<<fileName<<" 时出现问题"<<endl;
        exit(1);
    }
    return btmp;
}

void BufferManager::writtenBackToDisk(const char* fileName,blockNode* block){
    if(!block->dirty) // 这个块没有被修改过，所以它不需要写回文件
        return;
    else{ 
		// 写回文件
        FILE * fileHandle = NULL;
        if((fileHandle = fopen(fileName, "rb+")) != NULL){
            if(fseek(fileHandle, block->offsetNum*BLOCK_SIZE, 0) == 0){
                if(fwrite(block->address, block->usingSize+sizeof(size_t), 1, fileHandle) != 1){
                    cout<<"写入文件 "<<fileName<<" 时出现问题";
                    exit(1);
                }
            }
            else{
                cout<<"查找文件 "<<fileName<<" 时出现问题"<<endl;
                exit(1);
            }
            fclose(fileHandle);
        }
        else{
            cout << "打开文件 "<<fileName<<" 时出现问题"<<endl;
            exit(1);
        }
    }
}

void BufferManager::writtenBackToDiskAll(){
    blockNode *btmp = NULL;
    fileNode *ftmp = NULL;
    if(fileHead){
        for(ftmp = fileHead;ftmp != NULL;ftmp = ftmp ->nextFile){
            if(ftmp->blockHead){
                for(btmp = ftmp->blockHead;btmp != NULL;btmp = btmp->nextBlock){
                    if(btmp->preBlock)initBlock(*(btmp -> preBlock));
                    writtenBackToDisk(btmp->fileName, btmp);
                }
            }
        }
    }
}

blockNode* BufferManager::getNextBlock(fileNode* file,blockNode* block){
    if(block->nextBlock == NULL){
        if(block->ifbottom) block->ifbottom = false;
        return getBlock(file, block);
    }
    else{ 
		//block->nextBlock != NULL
        if(block->offsetNum == block->nextBlock->offsetNum - 1)
            return block->nextBlock;
        else //阻止列表的顺序不对
            return getBlock(file, block);
    }
}

blockNode* BufferManager::getBlockHead(fileNode* file){
    blockNode* btmp = NULL;
    if(file->blockHead != NULL){
        if(file->blockHead->offsetNum == 0){ 
			//第一个块的右偏移量
            btmp = file->blockHead;
        }
        else
            btmp = getBlock(file, NULL);
    }
    else // 如果文件没有块头，则为其获取一个新的块节点
        btmp = getBlock(file,NULL);
    return btmp;
}

blockNode* BufferManager::getBlockByOffset(fileNode* file, int offsetNumber){
    blockNode* btmp = NULL;
    if(offsetNumber == 0) return getBlockHead(file);
    else{
        btmp = getBlockHead(file);
        while( offsetNumber > 0){
            btmp = getNextBlock(file, btmp);
            offsetNumber --;
        }
        return btmp;
    }
}

void BufferManager::deleteFileNode(const char * fileName){
    fileNode* ftmp = getFile(fileName);
    blockNode* btmp = getBlockHead(ftmp);
    queue<blockNode*> blockQ;
    while (true) {
        if(btmp == NULL) return;
        blockQ.push(btmp);
        if(btmp->ifbottom) break;
        btmp = getNextBlock(ftmp,btmp);
    }
    totalBlock -= blockQ.size();
    while(!blockQ.empty()){
        initBlock(*blockQ.back());
        blockQ.pop();
    }
    if(ftmp->preFile) ftmp->preFile->nextFile = ftmp->nextFile;
    if(ftmp->nextFile) ftmp->nextFile->preFile = ftmp->preFile;
    if(fileHead == ftmp) fileHead = ftmp->nextFile;
    initFile(*ftmp);
    totalFile --;
}

void BufferManager::setPin(blockNode &block,bool pin){
    block.pin = pin;
    if(!pin)
        block.reference = true;
}

void BufferManager::setPin(fileNode &file,bool pin){
    file.pin = pin;
}

void BufferManager::setDirty(blockNode &block){
    block.dirty = true;
}

void BufferManager::cleanDirty(blockNode &block){
    block.dirty = false;
}

size_t BufferManager::getUsingSize(blockNode* block){
    return *(size_t*)block->address;
}

void BufferManager::setUsingSize(blockNode & block,size_t usage){
    block.usingSize = usage;
    memcpy(block.address,(char*)&usage,sizeof(size_t));
}

size_t BufferManager::getUsingSize(blockNode & block){
    return block.usingSize;
}

char* BufferManager::getContent(blockNode& block){
    return block.address + sizeof(size_t);
}

int BufferManager::getBlockSize() {
	//获取其他人可以使用的块的大小。其他人不能使用块头
	return BLOCK_SIZE - sizeof(size_t);
}
