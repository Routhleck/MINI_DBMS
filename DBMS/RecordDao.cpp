#include "stdafx.h"
#include "AppException.h"
#include "RecordDao.h"
#include "TimeUtil.h"
#include "CharUtil.h"

bool CRecordDao::Insert(CTableEntity &te, CRecordEntity &re)
{
	try
	{
		CFile file;
		// Open file
		if (file.Open(te.GetTrdPath(), CFile::modeWrite | CFile::shareDenyWrite) == FALSE)
		{
			return false;
		}
		// The cursor to the end of the file
		file.SeekToEnd();

		// Save record
		Write(file, te, re);

		// Close file
		file.Close();
		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		throw new CAppException(_T("Failed to save record!"));
	}
	catch (...)
	{
		throw new CAppException(_T("Failed to save record!"));
	}
	return false;
}

int CRecordDao::SelectAll(CTableEntity &te, RECORDARR &data)
{
	try
	{
		CFile file;
		// Open file
		if (file.Open(te.GetTrdPath(), CFile::modeRead | CFile::shareDenyNone) == FALSE)
		{
			return 0;
		}
		// The cursor to the end of the file
		file.SeekToBegin();

		int nCount = 0;
		while (true)
		{
			// Get the value to write
			CRecordEntity* pRecordEntity = new CRecordEntity();
			if (Read(file, te, *pRecordEntity) == true)
			{
				data.Add(pRecordEntity);
				nCount++;
			}
			else
			{
				delete pRecordEntity;
				pRecordEntity = NULL;
				break;// Exit the while loop
			}
		}
		// Close file
		file.Close();

		return nCount;
	}
	catch (CException* e)
	{
		e->Delete();
		throw new CAppException(_T("Failed to query record!"));
	}
	catch (...)
	{
		throw new CAppException(_T("Failed to query record!"));
	}

	return 0;
}

bool CRecordDao::Write(CFile &file, CTableEntity &te, CRecordEntity &re)
{
	try
	{
		int nFieldNum = te.GetFieldNum();
		for (int i = 0; i < nFieldNum; i++)
		{
			CFieldEntity* pField = te.GetFieldAt(i);

			CString strFieldName = pField->GetName();
			CString strVal = re.Get(*pField);

			switch (pField->GetType())
			{
			case CFieldEntity::DT_INTEGER:
			{
				int nVal = _wtoi(strVal);
				file.Write(&nVal, sizeof(int));
				break;
			}
			case CFieldEntity::DT_BOOL:
			{
				int nVal = _wtoi(strVal);
				file.Write(&nVal, sizeof(bool));
				break;
			}
			case CFieldEntity::DT_DOUBLE:
			{
				double dbVal = _wtof(strVal);
				file.Write(&dbVal, sizeof(double));
				break;
			}
			case CFieldEntity::DT_DATETIME:
			{
				SYSTEMTIME st = CTimeUtil::ToSystemTime(strVal);
				file.Write(&st, sizeof(SYSTEMTIME));
				break;
			}
			case CFieldEntity::DT_VARCHAR:
			{
				int nSize = sizeof(char) * pField->GetParam();

				char* pBuf = new char[nSize];
				CCharUtil::ToChars(pBuf, strVal, nSize);

				file.Write(pBuf, nSize);

				delete[] pBuf;
				break;
			}
			default:
			{
				throw new CAppException(_T("Field data type is unusual, save record failed!"));
			}
			}

		}
		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		throw new CAppException(_T("Failed to save record!"));
	}
	catch (...)
	{
		throw new CAppException(_T("Failed to save record!"));
	}

	return false;
}

bool CRecordDao::Read(CFile &file, CTableEntity &te, CRecordEntity &re)
{
	try
	{
		// Get field number and read the value of each field  one by one.
		int nFieldNum = te.GetFieldNum();
		for (int i = 0; i < nFieldNum; i++)
		{
			// Get field information.
			CFieldEntity* pField = te.GetFieldAt(i);
			CString strFieldName = pField->GetName();// Field name

			switch (pField->GetType())
			{
			case CFieldEntity::DT_INTEGER: // Integer
			{
				int nVal;
				if (file.Read(&nVal, sizeof(int)) == 0)
				{
					return false;
				}
				re.Put(strFieldName, nVal);
				break;
			}
			case CFieldEntity::DT_BOOL: // Boolean
			{
				bool bVal;
				if (file.Read(&bVal, sizeof(bool)) == 0)
				{
					return false;
				}
				re.Put(strFieldName, bVal);
				break;
			}
			case CFieldEntity::DT_DOUBLE: // Floating-point number
			{
				double dbVal;
				if (file.Read(&dbVal, sizeof(double)) == 0)
				{
					return false;
				}
				re.Put(strFieldName, dbVal);
				break;
			}
			case CFieldEntity::DT_DATETIME: // Time type
			{
				SYSTEMTIME st;
				if (file.Read(&st, sizeof(SYSTEMTIME)) == 0)
				{
					return false;
				}
				re.Put(strFieldName, st);
				break;
			}
			case CFieldEntity::DT_VARCHAR: // String type
			{
				int nSize = sizeof(char) * pField->GetParam();
				char* pBuf = new char[nSize];
				if (file.Read(pBuf, nSize) == 0)
				{
					return false;
				}
				re.Put(strFieldName, CCharUtil::ToString(pBuf, nSize));
				// Release cache
				delete[] pBuf;
				break;
			}
			default: // Other data types
			{
				throw new CAppException(_T("Field data type is unusual, read record failed!"));
			}
			}// end switch
		}// end for
		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		throw new CAppException(_T("Failed to read record!"));
	}
	catch (...)
	{
		throw new CAppException(_T("Failed to read record!"));
	}

	return false;
}