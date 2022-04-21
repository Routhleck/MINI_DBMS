#pragma once
#include "afx.h"

class CAppException
{
public:
	CAppException(void);
	CAppException(CString strError);
	~CAppException(void);

public:
	CString GetErrorMessage();
private:
	CString m_strError;
	int m_nCode;
};
