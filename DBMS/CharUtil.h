#pragma once
class CCharUtil
{
public:
	CCharUtil();
	~CCharUtil();
	static CString ToString(char* pSrc, const int nSize);
	static void ToChars(char * pDim, CString strSrc, const int nSize);
};

