#pragma once
#include "TableEntity.h"
#include "FieldEntity.h"

class CTableDao
{
public:
	CTableDao();
	~CTableDao();

	// Create table and save table information
	bool Create(const CString strFilePath, CTableEntity &te);
	// Add a table field
	bool AddField(const CString strFilePath, CFieldEntity &fe);
	// Get table information
	int GetTables(const CString strFilepath, TABLEARR &arr);
	// Get field information
	bool GetFields(const CString strFilepath, CTableEntity &te);
	// Alert table
	bool AlterTable(const CString strFilePath, CTableEntity &te);
	bool RenameTable(CString strFilePath, CString oldName, CTableEntity * e);
};

