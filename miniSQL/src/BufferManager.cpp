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
        initFile(file_pool[i]);//初始化文件池
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
        initBlock(block_pool[i]);//初始化块池
    }
}

BufferManager::~BufferManager(){
    writtenBackToDiskAll();//写回文件
    for (int i = 0; i < MAX_FILE_NUM; i ++){
        delete [] file_pool[i].fileName;
    }
    for (int i = 0; i < MAX_BLOCK_NUM; i ++){
        delete [] block_pool[i].address;
    }
}

//初始化文件
void BufferManager::initFile(fileNode &file){
    file.nextFile = NULL;
    file.preFile = NULL;
    file.blockHead = NULL;
    file.pin = false;
    memset(file.fileName,0,MAX_FILE_NAME);
}

//初始化块
void BufferManager::initBlock(blockNode &block){
    //初始化内存块总大小4096
    memset(block.address,0,BLOCK_SIZE);
    size_t init_usage = 0;
    //size_t大小足以保证存储内存中对象的大小
    memcpy(block.address, (char*)&init_usage, sizeof(size_t)); //设置块头
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

//返回文件fileNode
fileNode* BufferManager::getFile(const char * fileName, bool if_pin){
    blockNode * btmp = NULL;
    fileNode * ftmp = NULL;
    if(fileHead != NULL){
        //文件检索
        for(ftmp = fileHead;ftmp != NULL;ftmp = ftmp->nextFile){
            //比对文件名
            if(!strcmp(fileName, ftmp->fileName)){ 
				//fileNode已经在列表中
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
        ftmp->preFile = &file_pool[totalFile-1];//设置前驱节点
        totalFile ++;
    }
    else {
		// 若totalFile >= MAX_FILE_NUM,找到一个要替换的fileNode，写回属于该fileNode的块节点
        ftmp = fileHead;
        while(ftmp->pin){
            if(ftmp -> nextFile)
                ftmp = ftmp->nextFile;
            else {
				//池中没有足够的文件节点
                cout<<"文件池中没有足够的文件节点!";
                exit(2);
            }
        }
        for(btmp = ftmp->blockHead;btmp != NULL;btmp = btmp->nextBlock){
            if(btmp->preBlock){
                initBlock(*(btmp->preBlock));
                totalBlock --;//总块数减1
            }
            writtenBackToDisk(btmp->fileName,btmp);//写回文件
        }
        initFile(*ftmp);//初始化文件
    }
    if(strlen(fileName) + 1 > MAX_FILE_NAME){
        cout <<"文件名长于最大值 " << MAX_FILE_NAME<<endl;
        exit(3);
    }
    strncpy(ftmp->fileName, fileName,MAX_FILE_NAME);//设置文件名
    setPin(*ftmp, if_pin);//同步锁定状态
    return ftmp;
}

//返回BlockNode
blockNode* BufferManager::getBlock(fileNode * file,blockNode *position, bool if_pin){
    const char * fileName = file->fileName;
    blockNode * btmp = NULL;
    if(totalBlock == 0){// 现在列表中没有块
        btmp = &block_pool[0];
        totalBlock ++;//块总数加1
    }
    else if(totalBlock < MAX_BLOCK_NUM){ // 块池中存在空块
        for(int i = 0 ;i < MAX_BLOCK_NUM;i ++){
            if(block_pool[i].offsetNum == -1){
                btmp = &block_pool[i];
                totalBlock ++;//块总数加1
                break;
            }
            else
                continue;
        }
    }
    else{ // totalBlock >= MAX_BLOCK_NUM,这意味着没有空块，所以我们必须替换一个。
        int i = replaced_block;
        while (true){
            i ++;
            if(i >= totalBlock) i = 0;
            if(!block_pool[i].pin){//若该块没被锁定
                if(block_pool[i].reference == true)
                    block_pool[i].reference = false;
                else {
					//选择此块进行更换
                    btmp = &block_pool[i];
                    if(btmp->nextBlock) btmp -> nextBlock -> preBlock = btmp -> preBlock;
                    if(btmp->preBlock) btmp -> preBlock -> nextBlock = btmp -> nextBlock;
                    if(file->blockHead == btmp) file->blockHead = btmp->nextBlock;
                    replaced_block = i; //记录被替换的块，下次从下一个块开始。
                    
                    writtenBackToDisk(btmp->fileName, btmp);//写回文件
                    initBlock(*btmp);//初始化块
                    break;
                }
            }
            else // 此块已被锁定
                continue;
        }
    }
    //将块添加到块列表中
    if(position != NULL && position->nextBlock == NULL){//若列表非空，且下一个为空块
        btmp -> preBlock = position;
        position -> nextBlock = btmp;
        btmp -> offsetNum = position -> offsetNum + 1;
    }
    else if (position !=NULL && position->nextBlock != NULL){//若列表非空，且下一个为非空块
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
			// 如果文件有一个错误的块头,则更换块头
            file->blockHead -> preBlock = btmp;
            btmp->nextBlock = file->blockHead;
        }
        file->blockHead = btmp;
    }
    setPin(*btmp, if_pin);//设置锁定状态
    if(strlen(fileName) + 1 > MAX_FILE_NAME){
        cout << "文件名长于最大值 "<<MAX_FILE_NAME<<endl;
        exit(3);
    }
    strncpy(btmp->fileName, fileName, MAX_FILE_NAME);//设置文件名
    
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

//写回文件至硬盘,(指定文件名)
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

//写回全部文件至硬盘
void BufferManager::writtenBackToDiskAll(){
    blockNode *btmp = NULL;
    fileNode *ftmp = NULL;
    if(fileHead){//文件头不为空
        for(ftmp = fileHead;ftmp != NULL;ftmp = ftmp ->nextFile){
            if(ftmp->blockHead){//块头不为空
                for(btmp = ftmp->blockHead;btmp != NULL;btmp = btmp->nextBlock){
                    if(btmp->preBlock)initBlock(*(btmp -> preBlock));//初始化块
                    writtenBackToDisk(btmp->fileName, btmp);//写回文件
                }
            }
        }
    }
}

//获取下一个块BLOCK
blockNode* BufferManager::getNextBlock(fileNode* file,blockNode* block){
    if(block->nextBlock == NULL){//若后驱节点为空块
        if(block->ifbottom) block->ifbottom = false;// 文件节点结束的标志设为false
        return getBlock(file, block);
    }
    else{ 
		//若后驱节点为非空块
        if(block->offsetNum == block->nextBlock->offsetNum - 1)
            return block->nextBlock;
        else //阻止列表的顺序不对
            return getBlock(file, block);
    }
}

//获取块头BLockHead
blockNode* BufferManager::getBlockHead(fileNode* file){
    blockNode* btmp = NULL;
    if(file->blockHead != NULL){//若文件有块头
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

//获取块(指定offset)
blockNode* BufferManager::getBlockByOffset(fileNode* file, int offsetNumber){
    blockNode* btmp = NULL;
    if(offsetNumber == 0) return getBlockHead(file);//若偏移量为0
    else{
        btmp = getBlockHead(file);
        while( offsetNumber > 0){
            btmp = getNextBlock(file, btmp);//变为下一块
            offsetNumber --;//偏移量减1
        }
        return btmp;
    }
}

//删除文件节点
void BufferManager::deleteFileNode(const char * fileName){
    fileNode* ftmp = getFile(fileName);
    blockNode* btmp = getBlockHead(ftmp);
    queue<blockNode*> blockQ;
    while (true) {
        if(btmp == NULL) return;
        blockQ.push(btmp);//压入队列
        if(btmp->ifbottom) break;//若文件节点结束
        btmp = getNextBlock(ftmp,btmp);//指向下一个块
    }
    totalBlock -= blockQ.size();//减少总块数
    while(!blockQ.empty()){//当队列非空
        initBlock(*blockQ.back());//初始化块
        blockQ.pop();//从队列中弹出
    }
    if(ftmp->preFile) ftmp->preFile->nextFile = ftmp->nextFile;
    if(ftmp->nextFile) ftmp->nextFile->preFile = ftmp->preFile;
    if(fileHead == ftmp) fileHead = ftmp->nextFile;
    initFile(*ftmp);
    totalFile --;
}

//设置块PIN锁,若没有锁则reference,LRU更换标志为true
void BufferManager::setPin(blockNode &block,bool pin){
    block.pin = pin;
    if(!pin)
        block.reference = true;
}

//设置文件PIN锁
void BufferManager::setPin(fileNode &file,bool pin){
    file.pin = pin;
}

//设置此块为脏
void BufferManager::setDirty(blockNode &block){
    block.dirty = true;
}

//设置此块为非脏
void BufferManager::cleanDirty(blockNode &block){
    block.dirty = false;
}

//返回块使用地址空间大小
size_t BufferManager::getUsingSize(blockNode* block){
    return *(size_t*)block->address;
}

//设置块使用空间
void BufferManager::setUsingSize(blockNode & block,size_t usage){
    block.usingSize = usage;
    memcpy(block.address,(char*)&usage,sizeof(size_t));
}

//返回块使用的字节大小
size_t BufferManager::getUsingSize(blockNode & block){
    return block.usingSize;
}

//返回地址内容
char* BufferManager::getContent(blockNode& block){
    return block.address + sizeof(size_t);
}

//获取其他可以使用的块的大小。其他不能使用块头
int BufferManager::getBlockSize() {
	return BLOCK_SIZE - sizeof(size_t);
}
