#include "stdafx.h"
#include "TableDao.h"
#include "AppException.h"
#include "DataStructure.h"
#include "TableEntity.h"
CTableDao::CTableDao()
{
}


CTableDao::~CTableDao()
{
}

bool CTableDao::Create(const CString strFilePath, CTableEntity & te)
{
	try
	{
		CFile file;
		// Open file
		if (file.Open(strFilePath, CFile::modeWrite | CFile::shareDenyWrite) == FALSE)
		{
			return false;
		}
		// Write in the database table information to the file
		file.SeekToEnd();
		file.Write(&te.GetTable(), sizeof(Table));
		// Close file
		file.Close();
		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		throw new CAppException(_T("Failed to add table information!"));
	}
	catch (...)
	{
		throw new CAppException(_T("Failed to add table information!"));
	}

	return false;
}

bool CTableDao::AddField(const CString strFilePath, CFieldEntity & fe)
{
	try
	{
		CFile file;
		// Open file
		if (file.Open(strFilePath, CFile::modeWrite | CFile::shareDenyWrite) == FALSE)
		{
			return false;
		}

		// Add field informaiton
		file.SeekToEnd();
		Field fb = fe.GetField();
		file.Write(&fb, sizeof(Field));

		// Close file
		file.Close();
		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		throw new CAppException(_T("Failed to add field!"));
	}
	catch (...)
	{
		throw new CAppException(_T("Failed to add field!"));
	}
	return false;
}

int CTableDao::GetTables(const CString strFilepath, TABLEARR & arrTB)
{
	try
	{
		CFile file;
		// Open file
		if (file.Open(strFilepath, CFile::modeRead | CFile::shareDenyNone) == FALSE)
		{
			return false;
		}
		file.SeekToBegin();

		Table tb;
		int nCount = 0;
		// Read table information
		while (file.Read(&tb, sizeof(Table)) > 0)
		{
			CTableEntity* pTable = new CTableEntity();
			pTable->SetTable(tb);
			arrTB.Add(pTable);
			nCount++;
		}
		// Close file
		file.Close();
		return nCount;
	}
	catch (CException* e)
	{
		e->Delete();
		throw new CAppException(_T("Failed to query table information!"));
	}
	catch (...)
	{
		throw new CAppException(_T("Failed to query table information!"));
	}

	return 0;
}

bool CTableDao::GetFields(const CString strFilepath, CTableEntity &te)
{
	try
	{
		CFile file;
		// Open file
		if (file.Open(strFilepath, CFile::modeRead | CFile::shareDenyNone) == FALSE)
		{
			return false;
		}
		file.SeekToBegin();

		// Read field information
		Field fb;
		while (file.Read(&fb, sizeof(Field)) > 0)
		{
			CFieldEntity fe;
			fe.SetField(fb);
			te.AddField(fe);// Add field
		}

		// Close file
		file.Close();
		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		throw new CAppException(_T("Failed to read field information!"));
	}
	catch (...)
	{
		throw new CAppException(_T("Failed to read field information!"));
	}

	return false;
}

bool CTableDao::AlterTable(const CString strFilePath, CTableEntity &te)
{
	try
	{
		CFile file;
		// Open file
		if (file.Open(strFilePath, CFile::modeReadWrite | CFile::shareDenyWrite) == FALSE)
		{
			return false;
		}

		file.SeekToBegin();
		long lOffset = file.GetPosition();

		// Query table information according to the table name, then alter the it
		bool flag = false;
		Table tb;
		CString strName;// Table name
		while (file.Read(&tb, sizeof(Table)) > 0)
		{
			strName = CCharUtil::ToString(tb.name, sizeof(VARCHAR));
			// Compare the table name
			if (te.GetName().Compare(strName) == 0)
			{
				file.Seek(lOffset, CFile::begin);// The file pointer points to the position of the previous record
				file.Write(&te.GetTable(), sizeof(Table));// Alter the table information
				flag = true;
				break;
			}
			lOffset = file.GetPosition();
		}

		// Close file
		file.Close();
		return flag;
	}
	catch (CException* e)
	{
		e->Delete();
		throw new CAppException(_T("Failed to alert table!"));
	}
	catch (...)
	{
		throw new CAppException(_T("Failed to alert table!"));
	}
	return false;
}

bool CTableDao::RenameTable(CString strFilePath, CString oldName, CTableEntity *e) {
	try
	{
		CFile file;
		// Open file
		if (file.Open(strFilePath, CFile::modeReadWrite | CFile::shareDenyWrite) == FALSE)
		{
			return false;
		}

		file.SeekToBegin();
		long lOffset = file.GetPosition();

		bool flag = false;
		Table tb;
		CString strName;// Table name
		while (file.Read(&tb, sizeof(Table)) > 0)
		{
			strName = CCharUtil::ToString(tb.name, sizeof(VARCHAR));
			// Compare the table name
			if (oldName.Compare(strName) == 0)
			{
				file.Seek(lOffset, CFile::begin);// The file pointer points to the position of the previous record
				file.Write(& (e->GetTable()), sizeof(Table));// Alter the table information
				flag = true;
				break;
			}
			lOffset = file.GetPosition();
		}

		// Close file
		file.Close();
		return flag;
	}
	catch (CException* e)
	{
		e->Delete();
		throw new CAppException(_T("Failed to alter table!"));
	}
	catch (...)
	{
		throw new CAppException(_T("Failed to alter table!"));
	}
	return false;
}