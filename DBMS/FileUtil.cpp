#include "stdafx.h"
#include "FileUtil.h"
#include "AppException.h"

CFileUtil::CFileUtil(){}


CFileUtil::~CFileUtil(){}

bool CFileUtil::IsValidFile(CString strPath){
	try{
		CFile file;
		if (file.Open(strPath, CFile::modeRead | CFile::shareDenyNone) == TRUE){
			file.Close();
			return true;
		}
	}
	catch (CException* e){
		e->Delete();
		throw new CAppException(_T("Failed to open file"));
	}
	catch (...){
		throw new CAppException(_T("Failed to open file"));
	}
	return false;
}

bool CFileUtil::CreateFile(CString strFile){
	try{
		// Create all the folder of the fiel path
		for (int i = 0; i < strFile.GetLength(); i++){
			if ((_T('\\') == strFile.GetAt(i)
				|| _T('/') == strFile.GetAt(i))
				&& 2 != i
				){
				CString strDirectory;
				strDirectory = strFile.Left(i);
				if (!CreateDirectory(strDirectory, NULL) && 183 != GetLastError()){
					return false;
				}
			}
		}

		CFile file;
		if (!file.Open(strFile, CFile::modeCreate)){
			return false;
		}
		file.Close();

		return true;
	}
	catch (CException* e){
		e->Delete();
		throw new CAppException(_T("Failed to create file!"));
	}
	catch (...){
		throw new CAppException(_T("Failed to create file!"));
	}
	return false;
}
