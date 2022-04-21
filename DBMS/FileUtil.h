#pragma once
class CFileUtil
{
public:
	CFileUtil();
	~CFileUtil();
	static bool IsValidFile(CString strPath);
	static bool CreateFile(CString strFile);
};

