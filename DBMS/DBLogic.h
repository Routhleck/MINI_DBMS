#pragma once
#include "DBEntity.h"
#include "DBDao.h"
#include "FileLogic.h"
class CDBLogic
{
private:
	CDBDao daoDB;
	CFileLogic logicFile;
public:
	CDBLogic();
	~CDBLogic();
	bool CreateDatabase(CDBEntity & e);
	bool GetDatabase(CDBEntity & e);
	int GetDatabases(DBARR &arrDB);
};