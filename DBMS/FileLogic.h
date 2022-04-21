#pragma once
class CFileLogic
{
public:
	CFileLogic();
	~CFileLogic();
	CString GetDBFolder(CString strName);
	CString GetDBFile();
	CString GetAbsolutePath(const CString strRelativePath);
	CString GetTableFile(const CString strDBName);
	CString GetTbDefineFile(const CString strDBName, const CString strTableName);
	CString GetTbRecordFile(const CString strDBName, const CString strTableName);
};

