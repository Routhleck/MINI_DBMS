
// DBMSDoc.h : CDBMSDoc 类的接口
//


#pragma once
#include "DBEntity.h"
#include "TableEntity.h"
#include "RecordEntity.h"

class CDBMSDoc : public CDocument
{
protected: // 仅从序列化创建
	CDBMSDoc();
	DECLARE_DYNCREATE(CDBMSDoc)

private:
	CDBEntity *dbEntity;
	CTableEntity *selectedTB;
	CString strError;

	DBARR arrDB;
	TABLEARR arrTB;
	RECORDARR arrRecord;
// 特性
public:
	CString GetError();
	void SetError(CString error);
	void SetDBEntity(CDBEntity *e);
	void SetSelectedTB(CTableEntity *e);

	CDBEntity *GetDBEntity();
	CDBEntity * GetDBAt(int index);
	int GetDBNum();
	CTableEntity *GetTBAt(int index);
	int GetTableNum();
	CTableEntity *GetSelectedTB();

	CDBEntity * CreateDatabase(CString databaseName);
	void LoadDatabase();
	void LoadTables();
	void RenameTable(CString newName);
	CFieldEntity * AddField(CFieldEntity & field);
	CRecordEntity * InsertRecord(CRecordEntity & record);
	int GetRecordNum();
	CRecordEntity * GetRecord(int nIndex);
	void LoadRecord(void);
	CTableEntity * CreateTable(CString strName);

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CDBMSDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif 
	// SHARED_HANDLERS
};
