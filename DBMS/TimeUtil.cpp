#include "StdAfx.h"
#include "TimeUtil.h"
#include <comutil.h>

/**************************************************
[FunctionName]	ToLDatetimeString
[Function]	Convert SYSTEMTIME type into CString type
[Argument]	SYSTEMTIME t:System
[ReturnedValue] String with the form of yyyy-mm-dd hh:mm:ss SSS
**************************************************/
CString CTimeUtil::ToLDatetimeString(SYSTEMTIME t)
{
	CString strTime;
	strTime.Format(_T("%04u-%02u-%02u %02u:%02u:%02u %03u"), t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	return strTime;
}

/**************************************************
[FunctionName]	ToDatetimeString
[Function]	Convert SYSTEMTIME type into CString type
[Argument]	SYSTEMTIME t:System time
[ReturnedValue] String with the form of yyyy-mm-dd hh:mm:ss
**************************************************/
CString CTimeUtil::ToDatetimeString(SYSTEMTIME t)
{
	CString strTime;
	strTime.Format(_T("%04u-%02u-%02u %02u:%02u:%02u"), t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
	return strTime;
}

/**************************************************
[FunctionName]	ToDateString
[Function]	Convert SYSTEMTIME type into CString type
[Argument]	SYSTEMTIME t:System time
[ReturnedValue] String with the form of yyyy-mm-dd
**************************************************/
CString CTimeUtil::ToDateString(SYSTEMTIME t)
{
	CString strTime;
	strTime.Format(_T("%04u-%02u-%02u"), t.wYear, t.wMonth, t.wDay);
	return strTime;
}

/**************************************************
[FunctionName]	ToTimeString
[Function]	Convert SYSTEMTIME type into CString type
[Argument]	SYSTEMTIME t:System
[ReturnedValue] String with the form of hh:mm:ss
**************************************************/
CString CTimeUtil::ToTimeString(SYSTEMTIME t)
{
	CString strTime;
	strTime.Format(_T("%02u:%02u:%02u"), t.wHour, t.wMinute, t.wSecond);
	return strTime;
}

/**************************************************
[FunctionName]	ToLTimeString
[Function]	Convert SYSTEMTIME type into CString type
[Argument]	SYSTEMTIME t:System time
[ReturnedValue] String with the form of hh:mm:ss SSS
**************************************************/
CString CTimeUtil::ToLTimeString(SYSTEMTIME t)
{
	CString strTime;
	strTime.Format(_T("%02u:%02u:%02u %03u"), t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	return strTime;
}

/**************************************************
[FunctionName]	ToSystemTime
[Function]	Convert CString type ino SYSTEMTIME type
[Argument]	CString strTime: String with the form of yyyy-mm-dd hh:mm:ss SSS
[ReturnedValue] SYSTEMTIME
**************************************************/
SYSTEMTIME CTimeUtil::ToSystemTime(CString strTime)
{
	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	if (strTime.GetLength() > 0)
	{
		int nYear = _wtoi(strTime.Left(4));
		int nMonth = _wtoi(strTime.Mid(5, 2));
		int nDay = _wtoi(strTime.Mid(8, 2));
		int nHour = _wtoi(strTime.Mid(11, 2));
		int nMinute = _wtoi(strTime.Mid(14, 2));
		int nSecord = _wtoi(strTime.Mid(17, 2));
		int nMillisecond = _wtoi(strTime.Mid(20, 3));

		st.wYear = nYear;
		st.wMonth = nMonth;
		st.wDay = nDay;
		st.wHour = nHour;
		st.wMinute = nMinute;
		st.wSecond = nSecord;
		st.wMilliseconds = nMillisecond;
	}
	return st;
}

/**************************************************
[FunctionName]	GetCurrentTime
[Function]	Get the current system time
[Argument]	void
[ReturnedValue] SYSTEMTIME
**************************************************/
SYSTEMTIME CTimeUtil::GetCurrentTime()
{
	SYSTEMTIME t;
	::GetLocalTime(&t);
	return t;
}

/**************************************************
[FunctionName]	ToSystemTime
[Function]	Convert CTime type into SYSTEMTIME type
[Argument]	CTime t
[ReturnedValue] SYSTEMTIME
**************************************************/
SYSTEMTIME CTimeUtil::ToSystemTime(CTime t)
{
	SYSTEMTIME stCur;
	t.GetAsSystemTime(stCur);
	return stCur;
}

/**************************************************
[FunctionName]	ToCTime
[Function]	Convert SYSTEMTIME type into CTime type
[Argument]	SYSTEMTIME st
[ReturnedValue] CTime
**************************************************/
CTime CTimeUtil::ToCTime(SYSTEMTIME st)
{
	return CTime(st);
}