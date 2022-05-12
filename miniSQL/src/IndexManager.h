#ifndef __Minisql__IndexManager__
#define __Minisql__IndexManager__

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include "BufferManager.h"

using namespace std;

static BufferManager bm;

typedef int offsetNum; // 树节点的值

class API;
template<typename KeyType> class BPlusTree;

//属性类
class Attribute {
public:
	Attribute();
	~Attribute();
	Attribute(string nameInput, int typeInput, bool ifUniqueInput);
	string getIndex();
	void setIndex(string other);
	string getName();
	int getType();
	bool getUnique();
	void setUnique(bool other);
	int static const TYPE_FLOAT = -1;
	int static const TYPE_INT = 0;

private:
	string name;
	string index;  // 默认值为“”，表示无索引
	int type; //属性的类型，-1表示float, 0表示int，其他正整数表示char，值为char的个数)
	bool ifUnique;
};


//索引类
class IndexInfo {
public:
	IndexInfo();
	~IndexInfo();
	IndexInfo(string indexInput, string tableInput, string attributeInput, int typeInput);
	string getIndexName();
	string getTableName();
	string getAttribute();
	int getType();

private:
	string indexName;
	string tableName;
	string Attribute;
	int type;
};

//indexManager类
class IndexManager{
public:
	IndexManager();
	~IndexManager();
	IndexManager(API *apiInput);
	void createIndex(string filePath, int type); //通过filePath和type创建索引
	void dropIndex(string filePath, int type); //通过filePath和type删除索引
	offsetNum searchIndex(string filePath, string key, int type); //通过filePath和type搜索索引
	void insertIndex(string filePath, string key, offsetNum blockOffset, int type); //插入索引
	void deleteIndexByKey(string filePath, string key, int type); //通过key删除索引
	int getDegree(int type);
	int getKeySize(int type);
	void setKey(int type, string key);

private:
    typedef map<string, BPlusTree<int> *> intMap;
    typedef map<string, BPlusTree<string> *> stringMap;
    typedef map<string, BPlusTree<float> *> floatMap;

    int static const TYPE_FLOAT = Attribute::TYPE_FLOAT;
    int static const TYPE_INT = Attribute::TYPE_INT;

    API 	   *api; 
    intMap 	   indexIntMap;
    stringMap  IndexstringMap;
    floatMap   indexFloatMap;

	// 结构体 帮助将输入字符串转换为指定类型
    struct keyTmp {
        int intTmp;
        float floatTmp;
        string stringTmp;
    };
    struct keyTmp kt;
};

template<typename KeyType>
//b+树节点类
class BPlusTreeNode {
    friend class BPlusTree<KeyType>;
private:
    int m_degree;     // 节点度数

public:
    int keysNum;                            // keys的数量
    bool isLeaf;                            // 是否为叶子节点
    BPlusTreeNode *parent;                  // 双亲节点
    BPlusTreeNode *nextLeafNode;            // 下一个叶子结点
    std::vector<KeyType> keySet;            // 在节点中设置所有key
    std::vector<BPlusTreeNode *> childSet;  // 设置所有的孩子节点
    std::vector<offsetNum> valSet;          // 所有offsetNum值设置


    BPlusTreeNode(int degree, bool isNewLeaf = false);
    ~BPlusTreeNode();

    bool IsRoot();                                       // 是否为根节点
    bool SearchInNode(KeyType key, int &index);          // 在节点中搜索key,返回其位置
    BPlusTreeNode *Splite(KeyType &key, offsetNum &val); // 通过key来讲改节点分割
    int AddKey(KeyType key, offsetNum val = 0);          // 添加一个key和相应的offsetnum,并返回其值
    bool Delete(int index);                              // 删除一个key
};

template<typename KeyType>
//b+树类
class BPlusTree {
private:
    BPlusTreeNode<KeyType> *root;        // b+树的根节点
    std::string fileName;                // 创建B+树索引的文件
    int treeHeight;                      // 树的高度,若为空则为0
    int m_degree;
    int m_keySize;
    fileNode *m_fileNode;

public:
    BPlusTree(int degree, int keySize, std::string file);
    ~BPlusTree();

    BPlusTreeNode<KeyType> *SearchInTree(KeyType key, int &index); // 搜索key值得位置并返回其位置
    bool InsertKey(KeyType key, offsetNum val);   // 向树中插入一个key
    bool DeleteKey(KeyType key);                  // 删除一个key
    void DropTree(BPlusTreeNode<KeyType> *node);  // 删除节点及其子节点

    void ReadAllDisk();
    void ReadDiskNode(blockNode *btmp);
    void WrittenBackToDisk();

private:
    void RecursivelySplite(BPlusTreeNode<KeyType> *node);
    bool RecursivelyCombine(BPlusTreeNode<KeyType> *node);
};

//-----------------------------------------------------------------------------------------------------------
//-----------------------class member function of BPlusTreeNode----------------------------------------------

/**
 * constructor: 创建b+树节点
 * 
 * @parm degree: 新节点的度数
 * @parm isNewLeaf: 标注该节点是否为叶子节点
 * 
 */
template<typename KeyType>
BPlusTreeNode<KeyType>::BPlusTreeNode(int degree, bool isNewLeaf) : 
    m_degree(degree),
    keysNum(0),
    isLeaf(isNewLeaf),
    parent(nullptr),
    nextLeafNode(nullptr),
    keySet(std::vector<KeyType>(degree + 1, KeyType())),
    valSet(std::vector<offsetNum>(degree + 1, offsetNum())),
    childSet(std::vector<BPlusTreeNode *>(degree + 2, nullptr)) {}

/**
 * deconstructor
 * 
 */
template<typename KeyType>
BPlusTreeNode<KeyType>::~BPlusTreeNode()
{}

/**
 * 若是根节点->返回1
 * 不是->返回0
 * 
 */
template<typename KeyType>
inline bool BPlusTreeNode<KeyType>::IsRoot()
{
    return (parent == nullptr) ? true : false;
}

/**
 * 搜索在node中的key值
 * 
 * @parm key: 想要搜索的key值
 * @parm index: 我们想找到key值的位置,
 *              或者应当存在的位置 若不在node中
 * 
 * @return 返回是否在node中找到key值
 * 
 */
template<typename KeyType>
bool BPlusTreeNode<KeyType>::SearchInNode(KeyType key, int &index)
{
    if(keysNum == 0 || keySet[0] > key)
    {
        //empty node or key is out of range
        index = 0;
        return false;
    }
    else if(keySet[keysNum - 1] < key)
    {
        index = keysNum;
        return false;
    }
    else
    {
        //binary search to find the key
        int left = 0, right = keysNum - 1, middle = 0;
        while(left < right - 1)
        {
            middle = (left + right) / 2;
            if(keySet[middle] == key)
            {
                index = middle;
                return true;
            }
            else if(keySet[middle] < key)
            {
                left = middle;
            }
            else
            {
                right = middle;
            }
        }
        
        if(keySet[left] == key)
        {
            index = left;
            return true;
        }
        else if(keySet[right] == key)
        {
            index = right;
            return true;
        }
        else
        {
            index = right;
            return false;
        }
    }
}

/**
 * 使用key值来分离节点
 * 
 * @parm key: 添加到双亲节点的key值
 * 
 * @return PBlusTreeNode: 分离之后获得的新节点
 * 
 */
template<typename KeyType>
BPlusTreeNode<KeyType> *BPlusTreeNode<KeyType>::Splite(KeyType &key, offsetNum &val)
{
    if(keysNum == 0)
    {
        std::cout << "Empty node can not be splited!" << std::endl;
        return nullptr;
    }

    BPlusTreeNode *newNode = new BPlusTreeNode<KeyType>(m_degree, isLeaf);
    if(newNode == nullptr)
    {
        std::cout << "Problems in allocate memory in function splite" << std::endl;
        exit(0);
    }

    
    //考虑这些向量已经大小已经超出范围
    int begin = m_degree / 2 + 1;
    key = keySet[begin];
    val = valSet[begin];
    for(int i = begin; i <= m_degree; i++)
    {
        newNode->keySet[i - begin] = keySet[i];
        newNode->valSet[i - begin] = valSet[i];
        newNode->childSet[i - begin + 1] = childSet[i + 1];
        if(newNode->childSet[i - begin + 1] != nullptr)
        {
            newNode->childSet[i - begin + 1]->parent = newNode;
        }
        keySet[i] = KeyType();
        valSet[i] = offsetNum();
        childSet[i + 1] = nullptr;
    }
    newNode->parent = parent;
    keysNum = begin;
    newNode->keysNum = m_degree + 1 - begin;

    if(isLeaf == true)
    {
        newNode->nextLeafNode = nextLeafNode;
        nextLeafNode = newNode;
    }
    else
    {
        newNode->childSet[0] = new BPlusTreeNode<KeyType>(m_degree, false);
    }

    return newNode;
}

/**
 * 将key值添加至节点中
 * 
 * @parm key: 想要添加的key值
 * @parm val: key值的offsetnum
 * 
 * @return int: 添加完后的key值位置
 *              -1 若添加失败
 * 
 */
template<typename KeyType>
int BPlusTreeNode<KeyType>::AddKey(KeyType key, offsetNum val)
{
    int index = -1;

    if(SearchInNode(key, index) == true)
    {
        std::cout << "The key is in the node now!" << std::endl;
    }
    else
    {
        for(int i = keysNum - 1; i >= index; i--)
        {
            keySet[i + 1] = keySet[i];
            valSet[i + 1] = valSet[i];
            childSet[i + 2] = childSet[i + 1];
        }
        keySet[index] = key;
        valSet[index] = val;
        childSet[index + 1] = childSet[index];
        keysNum++;
    }

    return index;
}


/**
 * 删除一个key值
 * 
 * @pram index: 想要删除的在节点中的位置
 * 
 * @return bool: 是否成功删除key值
 * 
 */
template<typename KeyType>
bool BPlusTreeNode<KeyType>::Delete(int index)
{
    if(index < 0 || index >= keysNum)
    {
        std::cout << "The index is out of range!" << std::endl;
        return false;
    }
    else
    {
        for(int i = index; i < keysNum - 1; i++)
        {
            keySet[i] = keySet[i + 1];
            valSet[i] = valSet[i + 1];
            childSet[i + 1] = childSet[i + 2];
        }
        keySet[keysNum - 1] = KeyType();
        valSet[keysNum - 1] = offsetNum();
        childSet[keysNum] = nullptr;
        keysNum--;
        return true;
    }
}

//-----------------------class member function of BPlusTreeNode----------------------------------------------
//-----------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------
//-----------------------class member function of BPlusTree--------------------------------------------------

/**
 * constructor of BPlusTree
 * 
 * 
 */
template<typename KeyType>
BPlusTree<KeyType>::BPlusTree(int degree, int keySize, std::string file) : 
    m_degree(degree),
    root(new BPlusTreeNode<KeyType>(degree, true)),
    treeHeight(0),
    fileName(file),
    m_keySize(keySize),
    m_fileNode(bm.getFile(fileName.c_str()))
{
    ReadAllDisk();
}

/**
 * deconstructor
 * 
 */
template<typename KeyType>
BPlusTree<KeyType>::~BPlusTree()
{
    DropTree(root);
}

/**
 * 搜索key值并返回位置
 * 
 * @parm key: 想要找到的key值
 * @parm index: key值在
 *              我们找到的包含key的节点的位置
 * 
 * @return BPlusTreeNode *: 包含键的节点的指针
 *                          
 * 
 */
template<typename KeyType>
BPlusTreeNode<KeyType> *BPlusTree<KeyType>::SearchInTree(KeyType key, int &index)
{
    index = -1;
    BPlusTreeNode<KeyType> *searchNode = new BPlusTreeNode<KeyType>(m_degree, false);
    if(searchNode == nullptr)
    {
        std::cout << "Can't" << std::endl;
        exit(3);
    }
    searchNode = root;

    //到叶子节点
    while(searchNode->isLeaf == false)
    {
        if(searchNode->SearchInNode(key, index) == true)
        {
            index++;
        }
        searchNode = searchNode->childSet[index];
    }

    searchNode->SearchInNode(key, index);
    searchNode->isLeaf = true;
    return searchNode;
}

/**
 * 向树中插入一个key值
 * 
 * @parm key: 我们想添加的key值
 * @parm val: 记录的offsetnum
 * 
 */
template<typename KeyType>
bool BPlusTree<KeyType>::InsertKey(KeyType key, offsetNum val)
{
    int index = -1;
    BPlusTreeNode<KeyType> *searchNode = this->SearchInTree(key, index);

    if(searchNode->SearchInNode(key, index) == true)
    {
        std::cout << "The key is in the tree now!" << std::endl;
        return false;
    }
    else
    {
        searchNode->AddKey(key, val);
        if(searchNode->keysNum > searchNode->m_degree)
        {
            RecursivelySplite(searchNode);
        }
        return true;
    }
}

/**
 * 若父节点是否已满,则分裂参数节点
 * 然后，分裂它的父节点(递归过程)
 * 
 * @parm node: 第一个需要分裂的节点
 * 
 */
template<typename KeyType>
void BPlusTree<KeyType>::RecursivelySplite(BPlusTreeNode<KeyType> *node)
{
    KeyType key;
    offsetNum val;
    BPlusTreeNode<KeyType> *newNode = node->Splite(key, val);

    if(node->IsRoot() == true)
    {
        BPlusTreeNode<KeyType> *newRoot = new BPlusTreeNode<KeyType>(node->m_degree, false);
        if(newRoot == nullptr)
        {
            std::cout << "Memery error recursive splite" << std::endl;
            exit(3);
        }

        treeHeight++;
        newRoot->AddKey(key, val);
        node->parent = newRoot;
        newNode->parent = newRoot;
        newRoot->childSet[0] = node;
        newRoot->childSet[1] = newNode;
        root = newRoot;
    }
    else
    {
        int index = node->parent->AddKey(key, val);
        newNode->parent = node->parent;
        node->parent->childSet[index + 1] = newNode;
        if(node->parent->keysNum > node->parent->m_degree)
        {
            RecursivelySplite(node->parent);
        }
    }
}

/**
 * 删除树中的一个key值
 * 
 * @parm key: 想要删除的key值
 * 
 * @return bool: 成功删除为true,反之为false
 * 
 */
template<typename KeyType>
bool BPlusTree<KeyType>::DeleteKey(KeyType key)
{
    int index = -1;
    BPlusTreeNode<KeyType> *searchNode = this->SearchInTree(key, index);

    if(searchNode->keySet[index] != key)
    {
        std::cout << "The key is not in the tree!" << std::endl;
        return false;
    }
    else
    {
        searchNode->Delete(index);
        return RecursivelyCombine(searchNode);
    }
}

/**
 * 删除键之后，如果需要，递归地修改树
 * 
 * @parm node: 我们想要修改的节点
 * 
 * @return bool: 是否成功
 * 
 */
template <typename KeyType>
bool BPlusTree<KeyType>::RecursivelyCombine(BPlusTreeNode<KeyType> *node)
{
    int index = 0;
    int minNodeSize = (m_degree - 1) / 2;
    BPlusTreeNode<KeyType> *parent = node->parent;
    BPlusTreeNode<KeyType> *brother = nullptr;

    //得到该节点在父节点中的位置
    if(parent != nullptr && parent->SearchInNode(node->keySet[0], index) == true)
    {
        index++;
    }

    if(node == nullptr || node->keysNum >= minNodeSize)
    {
        //keys num is larger than the minmum keys num, don't need to combine
        return true;
    }
    else if(node->IsRoot() == true)
    {
		if (root->keysNum > 0) return true;
        if(root != nullptr && node->keysNum == 0 && node->isLeaf == false)
        {
			if (node == nullptr) cout << "node == nullptr" << endl;
			else cout << "fuck you!" << endl;
            root = root->childSet[0];
            root->parent = nullptr;
			delete node;
			if (root == nullptr) root = new BPlusTreeNode<KeyType>(m_degree, true);
            delete node;
        }
        return true;
    }
    else if(node->isLeaf == true)
    {
        //是叶子节点但不是根节点
        if(index == parent->keysNum)
        {
            //如果node节点是父亲节点的最后一个孩子
            //那么就选择node节点的左兄弟节点借值
            brother = parent->childSet[index - 1];
            if(brother->keysNum > minNodeSize)
            {
                //从兄弟节点借一个值
                //兄弟节点删除被借的值，父亲节点值相应变化
                //因为是叶节点，所以不用考虑孩子节点
                node->AddKey(brother->keySet[brother->keysNum - 1], brother->valSet[brother->keysNum - 1]);
                parent->keySet[index - 1] = node->keySet[0];
                parent->valSet[index - 1] = node->valSet[0];
                brother->Delete(brother->keysNum - 1);
                return true;
            }
            else
            {
                //合并两个节点
                for(int i = 0; i < node->keysNum; i++)
                {
                    brother->keySet[i + brother->keysNum] = node->keySet[i];
                    brother->valSet[i + brother->keysNum] = node->valSet[i];
                }
                brother->keysNum += node->keysNum;
                brother->nextLeafNode = node->nextLeafNode;

                parent->Delete(index - 1);
                delete node;
                return RecursivelyCombine(parent);
            }
        }
        else    //选择右边兄弟节点
        {
            brother = parent->childSet[index + 1];
            if(brother->keysNum > minNodeSize)
            {
                node->AddKey(brother->keySet[0], brother->valSet[0]);
                brother->Delete(0);
                parent->keySet[index] = brother->keySet[0];
                parent->valSet[index] = brother->valSet[0];
                return true;
            }
            else
            {
                for(int i = 0; i < brother->keysNum; i++)
                {
                    node->keySet[i + node->keysNum] = brother->keySet[i];
                    node->valSet[i + node->keysNum] = brother->valSet[i];
                }
                node->keysNum += brother->keysNum;
                node->nextLeafNode = brother->nextLeafNode;

                parent->Delete(index);
                delete brother;
                return RecursivelyCombine(parent);
            }
        }
    }
    else    //既不是根节点也不是叶节点
    {
        if(index == parent->keysNum)
        {
            brother = parent->childSet[index -1];
            if(brother->keysNum > minNodeSize - 1)
            {
                node->AddKey(parent->keySet[index - 1], parent->valSet[index - 1]);
                node->childSet[0] = brother->childSet[brother->keysNum];
                parent->keySet[index - 1] = brother->keySet[brother->keysNum - 1];
                parent->valSet[index - 1] = brother->valSet[brother->keysNum - 1];
                if(node->childSet[0] != nullptr)
                {
                    node->childSet[0]->parent = node;
                }
                brother->Delete(brother->keysNum - 1);

                return true;
            }
            else
            {
                brother->AddKey(parent->keySet[index - 1], parent->valSet[index - 1]);
                int brotherKeysNum = brother->keysNum;
                int i = 0;
                for(; i < node->keysNum; i++)
                {
                    brother->keySet[i + brotherKeysNum] = node->keySet[i];
                    brother->valSet[i + brotherKeysNum] = node->valSet[i];
                    brother->childSet[i + brotherKeysNum] = node->childSet[i];
                    brother->childSet[i + brotherKeysNum]->parent = brother;
                }
                brother->childSet[i + brotherKeysNum] = node->childSet[i];
                brother->childSet[i + brotherKeysNum]->parent = brother;
                brother->keysNum += i;

                parent->Delete(index - 1);
                delete node;
                return RecursivelyCombine(parent);
            }
        }
        else
        {
            brother = parent->childSet[index + 1];
            if(brother->keysNum > minNodeSize - 1)
            {
                node->AddKey(parent->keySet[index], parent->valSet[index]);
                node->childSet[node->keysNum] = brother->childSet[0];
                node->childSet[node->keysNum]->parent = node;
                parent->keySet[index] = brother->keySet[0];
                parent->valSet[index] = brother->valSet[0];

                BPlusTreeNode<KeyType> *temp = brother->childSet[1];
                brother->Delete(0);
                brother->childSet[0] = temp;
                return true;
            }
            else
            {
                node->AddKey(parent->keySet[index], parent->valSet[index]);
                int nodeKeysNum = node->keysNum;
                int i = 0;
                for(; i < brother->keysNum; i++)
                {
                    node->keySet[i + nodeKeysNum] = brother->keySet[i];
                    node->valSet[i + nodeKeysNum] = brother->valSet[i];
                    node->childSet[i + nodeKeysNum] = brother->childSet[i];
                    node->childSet[i + nodeKeysNum]->parent = node;
                }
                node->childSet[i + nodeKeysNum] = brother->childSet[i];
                node->childSet[i + nodeKeysNum]->parent = node;
                node->keysNum += i;

                parent->Delete(index);
                delete brother;
                return RecursivelyCombine(parent);
            }
        }
    }
}

/**
 * 删除一个节点及其所有子节点
 * 
 * @parm node: 想要删除的节点
 * 
 */
template<typename KeyType>
void BPlusTree<KeyType>::DropTree(BPlusTreeNode<KeyType> *node)
{
    if(node == nullptr) return;
    else if(node->isLeaf == false)
    {
        for(int i = 0; i < node->keysNum; i++)
        {
            DropTree(node->childSet[i]);
            node->childSet[i] = nullptr;
        }
    }

    delete node;
    return;
}

/**
 * 从硬盘中读取节点
 * 
 * @parm btmp: 我们想要读入的节点
 * 
 */
template<typename KeyType>
void BPlusTree<KeyType>::ReadDiskNode(blockNode *btmp)
{
    KeyType key;
    offsetNum val;
    int valSize = sizeof(offsetNum);
    char *indexBegin = bm.getContent(*btmp);
    char *valBegin = indexBegin + m_keySize;

    while(valBegin - bm.getContent(*btmp) < bm.getUsingSize(*btmp))
    {
        KeyType *keyPtr = reinterpret_cast<KeyType *>(indexBegin);
        offsetNum *valPtr = reinterpret_cast<offsetNum *>(valBegin);
        key = *keyPtr;
        val = *valPtr;

        InsertKey(key, val);
        valBegin += m_keySize + valSize;
        indexBegin += m_keySize + valSize;
    }
}

/**
 * 从硬盘中读取所有树
 * 
 */
template<typename KeyType>
void BPlusTree<KeyType>::ReadAllDisk()
{
    if(m_fileNode == nullptr) m_fileNode = bm.getFile(fileName.c_str());
    blockNode *btmp = bm.getBlockHead(m_fileNode);

    while(btmp != nullptr)
    {
        ReadDiskNode(btmp);
        if(btmp->ifbottom) break;
        btmp = bm.getNextBlock(m_fileNode, btmp);
    }
}

/**
 * 
 * 
 * 
 */
template<typename KeyType>
void BPlusTree<KeyType>::WrittenBackToDisk()
{
    if(m_fileNode == nullptr) m_fileNode = bm.getFile(fileName.c_str());
    blockNode *btmp = bm.getBlockHead(m_fileNode);
    BPlusTreeNode<KeyType> *leafHead = root;
    int valSize = sizeof(offsetNum);

    while(leafHead != nullptr && leafHead->isLeaf == false)
    {
        leafHead = leafHead->childSet[0];
    }
    while(leafHead != nullptr)
    {
        bm.setUsingSize(*btmp, 0);
        bm.setDirty(*btmp);
        for(int i = 0; i < leafHead->keysNum; i++)
        {
            char *key = reinterpret_cast<char *>( &(leafHead->keySet[i]) );
            char *val = reinterpret_cast<char *>( &(leafHead->valSet[i]) );

            memcpy(bm.getContent(*btmp) + bm.getUsingSize(*btmp), key, m_keySize);
            bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) + m_keySize);
            memcpy(bm.getContent(*btmp) + bm.getUsingSize(*btmp), val, valSize);
            bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) + valSize);
        }

        btmp = bm.getNextBlock(m_fileNode, btmp);
        leafHead = leafHead->nextLeafNode;
    }

    while(btmp->ifbottom == false)
    {
        bm.setUsingSize(*btmp, 0);
        bm.setDirty(*btmp);
        btmp = bm.getNextBlock(m_fileNode, btmp);
    }
}

//-----------------------class member function of BPlusTree--------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

#endif
