#pragma once
#include "TableEntity.h"
#include "RecordEntity.h"
#include "TableDao.h"
#include "RecordDao.h"
#include "FileLogic.h"

/**************************************************
[ClassName]	CRecordLogic
[Function]	Data record business logic class
**************************************************/
class CRecordLogic
{
public:
	// Create new rows in a table
	bool Insert(const CString strDBName, CTableEntity &te, CRecordEntity &re);
	// Retrieve all records from a specified table
	bool SelectAll(CTableEntity &te, RECORDARR &data);

private:
	CRecordDao m_daoRecord;	// Record data operation object
	CTableDao m_daoTable;	// Table data operation object
	CFileLogic m_fileLogic;	// File logic object
};