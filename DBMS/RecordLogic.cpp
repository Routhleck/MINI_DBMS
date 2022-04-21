#include "stdafx.h"
#include "RecordLogic.h"
#include "AppException.h"
#include "FileUtil.h"

bool CRecordLogic::Insert(const CString strDBName, CTableEntity &te, CRecordEntity &re)
{
	try
	{
		// Decide whether the file exists, if there is no,a file will be created.
		CString strTrdPath = te.GetTrdPath();
		if (CFileUtil::IsValidFile(strTrdPath) == false)
		{
			if (CFileUtil::CreateFile(strTrdPath) == false)
			{
				return false;
			}
		}

		// Insert a record
		if (m_daoRecord.Insert(te, re) == false)
		{
			return false;
		}

		// Modify record number
		int nRecordNum = te.GetRecordNum() + 1;
		te.SetRecordNum(nRecordNum);

		// Alert table information
		CString strTableFile = m_fileLogic.GetTableFile(strDBName);
		if (m_daoTable.AlterTable(strTableFile, te) == false)
		{
			return false;
		}

		return true;
	}
	catch (CAppException *e)
	{
		throw e;
	}

	return false;
}

bool CRecordLogic::SelectAll(CTableEntity &te, RECORDARR &data)
{
	try
	{
		// Read record
		if (m_daoRecord.SelectAll(te, data) == false)
		{
			return false;
		}

		return true;
	}
	catch (CAppException *e)
	{
		throw e;
	}

	return false;
}