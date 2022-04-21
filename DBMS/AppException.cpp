#include "stdafx.h"
#include "AppException.h"

CAppException::CAppException(void)
{
	m_strError = _T("");
	m_nCode = -1;
}

CAppException::CAppException(CString strError)
{
	m_strError = strError;
	m_nCode = 0;
}

CAppException::~CAppException(void){}

CString CAppException::GetErrorMessage()
{
	return m_strError;
}
