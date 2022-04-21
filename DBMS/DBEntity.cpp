#include "stdafx.h"
#include "DBEntity.h"
#include "CharUtil.h"

CDBEntity::CDBEntity(Database & db) {
	SetDatabase(db);
}

CDBEntity::CDBEntity(CString strName) {
	this->strName = strName;
	this->strFilePath = _T("");
	this->bType = false;
	::GetLocalTime(&tCtTime);
}

CDBEntity::CDBEntity()
{
	strName = _T("");
	bType = false;
	strFilePath = _T("");
	::GetLocalTime(&tCtTime);
}

CDBEntity::CDBEntity(const CDBEntity & e)
{
	this->bType = e.bType;
	this->strFilePath = e.strFilePath;
	this->strName = e.strName;
	this->tCtTime = e.tCtTime;
}

CDBEntity::~CDBEntity(){}

void CDBEntity::SetName(CString name) {
	strName = name;
}

CString CDBEntity::GetName() {
	return strName;
}

void CDBEntity::SetType(bool type) {
	bType = type;
}

bool CDBEntity::GetType() {
	return bType;
}

void CDBEntity::SetPath(CString path) {
	strFilePath = path;
}

CString CDBEntity::GetPath() {
	return strFilePath;
}

void CDBEntity::SetCrtTime(SYSTEMTIME time) {
	tCtTime = time;
}

SYSTEMTIME CDBEntity::GetCrtTime() {
	return tCtTime;
}

Database CDBEntity::GetDatabase()
{
	Database db;
	memset(&db, 0, sizeof(Database));// Initializes the memory space

	CCharUtil::ToChars(db.filepath, strFilePath, sizeof(VARCHAR));
	CCharUtil::ToChars(db.name, strName, sizeof(VARCHAR));
	db.creationTime = tCtTime;
	db.type = bType;

	return db;
}

void CDBEntity::SetDatabase(Database db)
{
	strName = CCharUtil::ToString(db.name, sizeof(VARCHAR));
	strFilePath = CCharUtil::ToString(db.filepath, sizeof(VARCHAR));
	bType = db.type;
	tCtTime = db.creationTime;
}

