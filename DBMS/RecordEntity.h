#pragma once
#include "FieldEntity.h"
      class CRecordEntity : public CObject{
private :
	CMapStringToString mapData;

public :
	CRecordEntity();
	CRecordEntity(CRecordEntity &e);
	~CRecordEntity();
	CRecordEntity & operator=(const CRecordEntity & e);

public :
	void Put(CString strFieldName, CString strValue);
	void Put(CString strFieldName, int nValue);
	void Put(CString strFieldName, double dbValue);
	void Put(CString strFieldName, SYSTEMTIME t);

	CString Get(CFieldEntity fe);

};

typedef CTypedPtrArray<CPtrArray, CRecordEntity*> RECORDARR;