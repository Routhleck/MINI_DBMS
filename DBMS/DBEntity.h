
#pragma once
#include "DataStructure.h"

class CDBEntity : public CObject{
private :
	CString		strName;
	CString		strFilePath;
	bool		bType;
	SYSTEMTIME	tCtTime;
public :
	CDBEntity(Database &db);
	CDBEntity(const CString strName);
	CDBEntity();
	CDBEntity(const CDBEntity& e);
	~CDBEntity();
public:
	void SetName(CString name);
	void SetType(bool type);
	void SetPath(CString path);
	void SetCrtTime(SYSTEMTIME time);

	CString GetName();
	bool GetType();
	CString GetPath();
	SYSTEMTIME GetCrtTime();

	Database GetDatabase();
	void SetDatabase(Database db);
};

typedef CTypedPtrArray<CPtrArray, CDBEntity*> DBARR;