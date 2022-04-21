#include "stdafx.h"
#include "TableLogic.h"
#include "AppException.h"
#include "FileUtil.h"
CTableLogic::CTableLogic()
{
	}


CTableLogic::~CTableLogic()
{
}

int CTableLogic::GetTables(const CString strDBName, TABLEARR &arrTables)
{
	int nCount = 0;

	try
	{
		// Get the table description file
		CString strTableFile = fileLogic.GetTableFile(strDBName);

		// Query talbe information
		nCount = daoTB.GetTables(strTableFile, arrTables);

		// Read the table structure from the table definition file one by one
		for (int i = 0; i < nCount; i++)
		{
			CTableEntity* pTable = arrTables.GetAt(i);
			daoTB.GetFields(pTable->GetTdPath(), *pTable);
		}
	}
	catch (CAppException e)
	{
		throw e;
	}

	return nCount;
}

bool CTableLogic::CreateTable(const CString strDBName, CTableEntity &te)
{
	try
	{
		CString strTableFile = fileLogic.GetTableFile(strDBName);
		if (CFileUtil::IsValidFile(strTableFile) == false)
		{
			if (CFileUtil::CreateFile(strTableFile) == false)
			{
				return false;
			}
		}

		te.SetTdPath(fileLogic.GetTbDefineFile(strDBName, te.GetName()));

		te.SetTrdPath(fileLogic.GetTbRecordFile(strDBName, te.GetName()));

		if (daoTB.Create(strTableFile, te) == false)
		{
			return false;
		}

		return true;
	}
	catch (CAppException* e)
	{
		throw e;
	}

	return false;
}

bool CTableLogic::RenameTable(const CString strDBName, CString oldTableName , CTableEntity *te) {
	try
	{
		CString strTableFile = fileLogic.GetTableFile(strDBName);

		te->SetTdPath(fileLogic.GetTbDefineFile(strDBName, te->GetName()));

		te->SetTrdPath(fileLogic.GetTbRecordFile(strDBName, te->GetName()));

		if (!daoTB.RenameTable(strTableFile, oldTableName , te))
		{
			return false;
		}
		if (CFileUtil::IsValidFile(fileLogic.GetTbDefineFile(strDBName, oldTableName)))
			CFile::Rename(fileLogic.GetTbDefineFile(strDBName, oldTableName), te->GetTdPath());

		if (CFileUtil::IsValidFile(fileLogic.GetTbRecordFile(strDBName, oldTableName)))
			CFile::Rename(fileLogic.GetTbRecordFile(strDBName, oldTableName), te->GetTrdPath());
		return true;
	}
	catch (CAppException* e)
	{
		throw e;
	}

	return false;
}

bool CTableLogic::AddField(const CString strDBName, CTableEntity &te, CFieldEntity &fe)
{
	try
	{
		// Decide whether the file exists, if there is no,a file will be created.
		CString strTdfPath = te.GetTdPath();
		if (CFileUtil::IsValidFile(strTdfPath) == false)
		{
			if (CFileUtil::CreateFile(strTdfPath) == false)
			{
				return false;
			}
		}

		// Save field information
		if (daoTB.AddField(strTdfPath, fe) == false)
		{
			return false;
		}
		// Add field
		te.AddField(fe);

		// Update modify time
		SYSTEMTIME tTime;
		::GetLocalTime(&tTime);
		te.SetLMTime(tTime);

		// Alert table information
		CString strTableFile = fileLogic.GetTableFile(strDBName);
		if (daoTB.AlterTable(strTableFile, te) == false)
		{
			return false;
		}
		return true;
	}
	catch (CAppException* e)
	{
		throw e;
	}

	return false;
}