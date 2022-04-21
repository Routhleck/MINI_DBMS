#include "stdafx.h"
#include "DBDao.h"
#include "AppException.h"
#include "DataStructure.h"
#include "CharUtil.h"

CDBDao::CDBDao(){

}

CDBDao::~CDBDao(){}


bool CDBDao::Create(CString strFilepath, CDBEntity & e){
	try
	{
		CFile file;
			if (file.Open(strFilepath,  CFile::modeWrite | CFile::shareDenyWrite) == FALSE)
			{
				return false;
			}
		
		file.SeekToEnd();
		file.Write(&e.GetDatabase(), sizeof(Database));
		file.Close();

		return true;
	}
	catch (CException* e)// Catch exceptions
	{
		// Delete the exception information
		e->Delete();
		// Throw a custom exception
		throw new CAppException(_T("Failed to create the database file!"));
	}
	catch (...)	// Catch other exceptions
	{
		// Throw a custom exception
		throw new CAppException(_T("Failed to create the database file!"));
	}

	return false;
}


bool CDBDao::GetDatabase(CString filePath,CDBEntity &e){
	try{
		CString strDBName = e.GetName();
		if (strDBName.GetLength() == 0){
			return false;
		}

		CFile file;
		if (file.Open(filePath, CFile::modeRead | CFile::shareDenyNone) == FALSE){
			return false;
		}

		bool bExist = false;
		Database sdb;

		file.SeekToBegin();
		while (file.Read(&sdb, sizeof(Database)) > 0){
			CString strName = CCharUtil::ToString(sdb.name, sizeof(VARCHAR));
			if (strName.CompareNoCase(strDBName) == 0){
				e.SetDatabase(sdb);
				bExist = true;
				break;
			}
		}
		file.Close();

		return bExist;
	}
	catch (CException* e){
		e->Delete();
		throw new CAppException(_T("Failed to create the database file!"));
	}
	catch (...){
		throw new CAppException(_T("Failed to create the database file!"));
	}
	return false;
}

int CDBDao::GetDatabases(CString filePath, DBARR & arrDB)
{
	try {
		CFile file;
		if (file.Open(filePath, CFile::modeRead | CFile::shareDenyNone) == FALSE) {
			return false;
		}
		Database sdb;
		file.SeekToBegin();
		int count = 0;
		while (file.Read(&sdb, sizeof(Database)) > 0) {
			CDBEntity *db = new CDBEntity(sdb);
			arrDB.Add(db);
			count++;
		}
		file.Close();
		return count;
	}
	catch (CException* e) {
		e->Delete();
		throw new CAppException(_T("Failed to create the database file!"));
	}
	catch (...) {
		throw new CAppException(_T("Failed to create the database file!"));
	}
}
