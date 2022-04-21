#pragma once
#include "DataStructure.h"
#include "FieldEntity.h"

class CTableEntity : public CObject{
private :
	CString		strName;
	int			nRecordNum;
	CString		strTdPath;
	CString		strTrdPath;
	CString		strTicPath;
	CString		strTidPath;
	SYSTEMTIME	tCtTime;
	SYSTEMTIME	tLMTime;
	FIELDARRAY	arrFields;
public :
	CTableEntity(CString strName);
	CTableEntity();
	CTableEntity(CTableEntity *);
	~CTableEntity();
public :
	Table GetTable();
	void SetTable(Table tb);
	CFieldEntity* AddField(CFieldEntity &field);
	CFieldEntity* GetFieldAt(int index);
	void SetName(CString name);
	void SetTdPath(CString path);
	void SetTrdPath(CString path);
	void SetTicPath(CString path);
	void SetTidPath(CString path);
	void SetCtTime(SYSTEMTIME time);
	void SetLMTime();
	void SetLMTime(SYSTEMTIME time);
	void SetRecordNum(int num);

	CString GetName();
	CString GetTdPath();
	CString GetTrdPath();
	CString GetTicPath();
	CString GetTidPath();
	SYSTEMTIME GetCtTime();
	SYSTEMTIME GETLMTime();

	int GetRecordNum();
	int GetFieldNum();
};

typedef CTypedPtrArray<CPtrArray, CTableEntity*> TABLEARR;