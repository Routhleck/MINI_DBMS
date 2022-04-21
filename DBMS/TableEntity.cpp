#include "stdafx.h"
#include "TableEntity.h"
#include "CharUtil.h"

CTableEntity::CTableEntity(CString strName){
	this->strName = strName;
	strTdPath = _T("");
	strTrdPath = _T("");
	strTicPath = _T("");
	strTidPath = _T("");
	nRecordNum = 0;
	::GetLocalTime(&tCtTime);
	tLMTime = tCtTime;
}

CTableEntity::CTableEntity(){
	strName = _T("");
	strTdPath = _T("");
	strTrdPath = _T("");
	strTicPath = _T("");
	strTidPath = _T("");
	nRecordNum = 0;
	::GetLocalTime(&tCtTime);
	tLMTime = tCtTime;
}

CTableEntity::CTableEntity( CTableEntity *e)
{
	int count = e->GetFieldNum();
	for (int i = 0; i < count;i++) {
		this->arrFields.Add(e->GetFieldAt(i));
	}
	this->strName = e->GetName();
	this->strTdPath = e->GetTdPath();
	this->strTrdPath = e->GetTrdPath();
	this->strTicPath = e->GetTicPath();
	this->strTidPath = e->GetTidPath();
	this->nRecordNum = 0;
	this->tCtTime = e->GetCtTime();
	::GetLocalTime(&tLMTime);
}

CTableEntity::~CTableEntity(){}

Table CTableEntity::GetTable(){
	Table tb;
	CCharUtil::ToChars(tb.name, strName, sizeof(VARCHAR));
	tb.field_num = arrFields.GetCount();
	tb.record_num = nRecordNum;
	CCharUtil::ToChars(tb.tdf, strTdPath, sizeof(VARCHAR));
	CCharUtil::ToChars(tb.trd, strTrdPath, sizeof(VARCHAR));
	tb.crtime = tCtTime;
	tb.lmtime = tLMTime;

	return tb;
}

void CTableEntity::SetTable(Table tb)
{
	strName = CCharUtil::ToString(tb.name, sizeof(VARCHAR));
	nRecordNum = tb.record_num;
	strTdPath = CCharUtil::ToString(tb.tdf, sizeof(VARCHAR));
	strTrdPath = CCharUtil::ToString(tb.trd, sizeof(VARCHAR));
	tCtTime = tb.crtime;
	tLMTime = tb.lmtime;
}

CFieldEntity * CTableEntity::AddField(CFieldEntity & field)
{
	CFieldEntity* pField = new CFieldEntity(field);
	arrFields.Add(pField);

	return pField;
}

CFieldEntity * CTableEntity::GetFieldAt(int index)
{
	return arrFields.GetAt(index);
}

void CTableEntity::SetName(CString name) {
	strName = name;
}

void CTableEntity::SetTdPath(CString path) {
	strTdPath = path;
}

void CTableEntity::SetTrdPath(CString path) {
	strTrdPath = path;
}

void CTableEntity::SetTicPath(CString path) {
	strTicPath = path;
}

void CTableEntity::SetTidPath(CString path) {
	strTidPath = path;
}

void CTableEntity::SetCtTime(SYSTEMTIME time) {
	tCtTime = time;
}

void CTableEntity::SetLMTime() {
	::GetLocalTime(&tLMTime);
}

void CTableEntity::SetLMTime(SYSTEMTIME time)
{
	this->tLMTime = time;
}

void CTableEntity::SetRecordNum(int num)
{
	nRecordNum = num;
}

CString CTableEntity::GetName() {
	return strName;
}

CString CTableEntity::GetTdPath() {
	return strTdPath;
}

CString CTableEntity::GetTrdPath() {
	return strTrdPath;
}

CString CTableEntity::GetTicPath() {
	return strTicPath;
}

CString CTableEntity::GetTidPath() {
	return strTidPath;
}

SYSTEMTIME CTableEntity::GetCtTime() {
	return tCtTime;
}

SYSTEMTIME CTableEntity::GETLMTime() {
	return tLMTime;
}

int CTableEntity::GetRecordNum()
{
	return nRecordNum;
}

int CTableEntity::GetFieldNum()
{
	return arrFields.GetCount();
}
