#include "stdafx.h"
#include "RecordEntity.h"
#include "TimeUtil.h"
#include "FieldEntity.h"
CRecordEntity::CRecordEntity() {

}

CRecordEntity::CRecordEntity(CRecordEntity &e) {
	POSITION pos = e.mapData.GetStartPosition();
	CString strFieldName, strValue;
	while (pos)
	{
		e.mapData.GetNextAssoc(pos, strFieldName, strValue);
		mapData.SetAt(strFieldName, strValue);
	}
}

CRecordEntity::~CRecordEntity(){}

CRecordEntity &CRecordEntity::operator =(const CRecordEntity &e)
{
	CString strFieldName, strValue;
	POSITION ipos = e.mapData.GetStartPosition();
	while (ipos != NULL)
	{
		e.mapData.GetNextAssoc(ipos, strFieldName, strValue);
		mapData.SetAt(strFieldName, strValue);
	}

	return *this;
}

void CRecordEntity::Put(CString strFieldName, CString strValue)
{
	mapData.SetAt(strFieldName, strValue);
}

void CRecordEntity::Put(CString strFieldName, int nValue)
{
	CString strValue;
	strValue.Format(_T("%d"), nValue);
	mapData.SetAt(strFieldName, strValue);
}

void CRecordEntity::Put(CString strFieldName, double dbValue)
{
	CString strValue;
	strValue.Format(_T("%f"), dbValue);
	mapData.SetAt(strFieldName, strValue);
}

void CRecordEntity::Put(CString strFieldName, SYSTEMTIME t)
{
	mapData.SetAt(strFieldName, CTimeUtil::ToLDatetimeString(t));
}

CString CRecordEntity::Get(CFieldEntity fe) {

	CString strVal;
	if (mapData.Lookup(fe.GetName(), strVal) == FALSE)
	{
		// Get the default value
		strVal = fe.GetDefaultValue();
	}
	return strVal;
}
