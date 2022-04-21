#include "stdafx.h"
#include "FieldEntity.h"

CFieldEntity::CFieldEntity()
{
}

CFieldEntity::CFieldEntity(CFieldEntity &e)
{
	strName = e.GetName();
	nType = e.GetType();
	nParam = e.GetParam();
	tLMTime = e.GetLMTime();
	nIntegrities = e.GetIntegerities();
	defaultVal = e.GetDefaultValue();
}

CFieldEntity::CFieldEntity(CString strName, int nType, int nParam, int nIntegrities)
{
	this->strName = strName;
	this->nType = nType;
	this->nParam = nParam;
	this->nIntegrities = nIntegrities;
	::GetLocalTime(&tLMTime);
}

CFieldEntity::~CFieldEntity(){}

void CFieldEntity::SetField(Field fd)
{
	strName = CCharUtil::ToString(fd.name, sizeof(VARCHAR));
	tLMTime = fd.lmtime;
	nType = fd.type;
	nParam = fd.param;
	defaultVal = CCharUtil::ToString(fd.defaultVal, sizeof(VARCHAR));
	nIntegrities = fd.integrities;
}

Field CFieldEntity::GetField()
{
	Field fd;
	memset(&fd, 0, sizeof(Field));

	CCharUtil::ToChars(fd.name, strName, sizeof(VARCHAR));
	CCharUtil::ToChars(fd.defaultVal, defaultVal, sizeof(VARCHAR));
	fd.lmtime = tLMTime;
	fd.type = nType;
	fd.param = nParam;
	fd.integrities = nIntegrities;
	return fd;
}

CString CFieldEntity::GetTypeName(int nDataType)
{
	CString strName = _T("");
	switch (nDataType)
	{
	case CFieldEntity::DT_INTEGER:	// INT type
	{
		strName = _T("INT");
		break;
	}
	case CFieldEntity::DT_BOOL:		// BOOL type
	{
		strName = _T("BOOL");
		break;
	}
	case CFieldEntity::DT_DOUBLE:	// DOUBLE type
	{
		strName = _T("DOUBLE");
		break;
	}
	case CFieldEntity::DT_VARCHAR:	// VRACHAR type
	{
		strName = _T("VARCHAR");
		break;
	}
	case CFieldEntity::DT_DATETIME:	// DATETIME type
	{
		strName = _T("DATATIME");
		break;
	}
	default:
		break;
	}
	return strName;
}

void CFieldEntity::SetName(CString name) {
	strName = name;
}

void CFieldEntity::SetType(int type){
	nType = type;
}

void CFieldEntity::SetParam(int param){
	nParam = param;
}

void CFieldEntity::SetLMTime(SYSTEMTIME time){
	tLMTime = time;
}

void CFieldEntity::SetIntegerities(int integerites)
{
	this->nIntegrities = integerites;
}

void CFieldEntity::SetDefaultValue(CString val)
{
	this->defaultVal = val;
}

void CFieldEntity::SetIsPrimaryKey(BOOL primaryKey)
{
	isPrimaryKey = primaryKey;
}


BOOL CFieldEntity::IsPrimaryKey()
{
	return isPrimaryKey;
}

CString CFieldEntity::GetDefaultValue()
{
	return defaultVal;
}

CString CFieldEntity::GetName() {
	return strName;
}

int CFieldEntity::GetType() {
	return nType;
}

int CFieldEntity::GetParam() {
	return nParam;
}

SYSTEMTIME CFieldEntity::GetLMTime() {
	return tLMTime;
}

int CFieldEntity::GetIntegerities()
{
	return nIntegrities;
}
