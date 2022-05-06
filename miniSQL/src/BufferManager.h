#ifndef __Minisql__BufferManager__
#define __Minisql__BufferManager__

#include <queue>
#include <string>
#include <iostream>
#include <ctime>

#define MAX_FILE_NUM 100
#define MAX_BLOCK_NUM 500
#define MAX_FILE_NAME 100

using namespace std;

static int replaced_block = -1;

struct blockNode{
	int offsetNum; // 块列表中的偏移量
	bool pin;  // 此块被锁定的标志
	bool ifbottom; // 文件节点结束的标志
	char* fileName; // 块节点所属的文件名

	char *address; // 内容地址
	blockNode * preBlock;
	blockNode * nextBlock;
	bool reference; // LRU更换标志
	bool dirty; // 表示此块是脏的标志，稍后需要将其写回磁盘
	size_t usingSize; // 块使用的字节大小。块的总大小为BLOCK_SIZE。这个值存储在块头部。
};

//文件节点B+树
struct fileNode{
	char *fileName;
	bool pin; // 此文件被锁定的标志
	blockNode *blockHead;
	fileNode * nextFile;
	fileNode * preFile;
};


class BufferManager{
public:
	BufferManager();
	~BufferManager();
	void deleteFileNode(const char * fileName); //删除缓冲区中的文件
	fileNode* getFile(const char* fileName, bool if_pin = false); //从缓冲区中获取文件
	void setDirty(blockNode & block); //设置脏块提醒磁盘回写
	void setPin(blockNode & block, bool pin); //将被锁定的文件设置为块
	void setPin(fileNode & file, bool pin);  //将被锁定的文件设置为文件
	void setUsingSize(blockNode & block, size_t usage);
	size_t getUsingSize(blockNode & block);
	char* getContent(blockNode& block);
	static int getBlockSize();

	blockNode* getNextBlock(fileNode * file, blockNode* block);
	blockNode* getBlockHead(fileNode* file);
	blockNode* getBlockByOffset(fileNode* file, int offestNumber);

private:
    fileNode *fileHead;
    fileNode file_pool[MAX_FILE_NUM];
    blockNode block_pool[MAX_BLOCK_NUM];
    int totalBlock; // 已使用的块的数量，这意味着该块在列表中。
    int totalFile; // 已使用的文件数，这意味着该文件在列表中。
    void initBlock(blockNode & block); //初始化块
    void initFile(fileNode & file); //初始化文件
    blockNode* getBlock(fileNode * file,blockNode* position,bool if_pin = false);
    void writtenBackToDiskAll(); //将内存中的所有块写回磁盘
    void writtenBackToDisk(const char* fileName, blockNode* block); //将设计好的块写入磁盘
    void cleanDirty(blockNode &block); //通过写回磁盘来清除脏块
    size_t getUsingSize(blockNode* block);
    static const int BLOCK_SIZE = 4096;

};

extern clock_t start;

#endif
