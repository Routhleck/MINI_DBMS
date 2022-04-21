#include "stdafx.h"
#include "CharUtil.h"


CCharUtil::CCharUtil()
{
}


CCharUtil::~CCharUtil(){}


CString CCharUtil::ToString(char* src, const int size)
{
	char * buffer = new char[size + 1];
	for (int i = 0; i < size; i++)
	{
		*(buffer + i) = *(src + i);
	}
	buffer[size] = NULL;

	int  nLen = ::MultiByteToWideChar(CP_ACP, 0, buffer, -1, NULL, 0);
	wchar_t *  wide_buffer = new  wchar_t[nLen + 1];
	memset(wide_buffer, 0, (nLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, buffer, -1, (LPWSTR)wide_buffer, nLen);

	CString str(wide_buffer);

	delete[] wide_buffer;
	delete[] buffer;

	return str;
}

void CCharUtil::ToChars(char* pDim, CString strSrc, const int nSize)
{
	// Initialize the pDim memory
	memset(pDim, 0, nSize);

	// Get the pointer to the strSrc string
	wchar_t* pSrc = strSrc.GetBuffer(strSrc.GetLength());
	// Get the length of the pSrc string
	int  nLen = ::WideCharToMultiByte(CP_ACP, 0, pSrc, -1, NULL, 0, NULL, NULL);
	// According to the number of bytes dynamically allocated memory
	char* pBuf = new char[nLen];

	// Convert wide byte into multibyte
	::WideCharToMultiByte(CP_ACP, 0, pSrc, -1, pBuf, nLen, NULL, NULL);

	if (nLen > nSize)
	{
		nLen = nSize;
	}

	for (int i = 0; i < nLen; i++)
	{
		*(pDim + i) = *(pBuf + i);
	}
	// Release cache
	strSrc.ReleaseBuffer();
	// Release the dynamically allocated memory
	delete[] pBuf;
}