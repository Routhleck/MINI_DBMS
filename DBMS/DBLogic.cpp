#include "stdafx.h"
#include "DBLogic.h"
#include "AppException.h"
#include "FileUtil.h"

CDBLogic::CDBLogic(){}

CDBLogic::~CDBLogic(){}

bool CDBLogic::CreateDatabase(CDBEntity & e){
	try{
		CString strDBFile = logicFile.GetDBFile();
		if (!CFileUtil::IsValidFile(strDBFile)){
			if (!CFileUtil::CreateFile(strDBFile)){
				return false;
			}
		}

		if (daoDB.GetDatabase(strDBFile, e)){
			return false;
		}

		CString strDBFolder = logicFile.GetDBFolder(e.GetName());
		e.SetPath(strDBFolder);
		if (!daoDB.Create(strDBFile, e)){
			return false;
		}
	}
	catch (CAppException* e){
		throw e;
	}
	catch (...){
		throw new CAppException(_T("Failed to create database!"));
	}
	return true;
}


bool CDBLogic::GetDatabase(CDBEntity & e){
	try{
		return daoDB.GetDatabase(logicFile.GetDBFile(), e);
	}
	catch (CAppException* e){
		throw e;
	}
	catch (...){
		throw new CAppException(_T("Failed to create database!"));
	}
	return false;
}

int CDBLogic::GetDatabases(DBARR & arrDB)
{
	try {
		return daoDB.GetDatabases(logicFile.GetDBFile(), arrDB);
	}
	catch (CAppException* e) {
		throw e;
	}
	catch (...) {
		throw new CAppException(_T("Failed to create database!"));
	}
	return false;
}
