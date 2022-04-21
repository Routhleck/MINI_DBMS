#pragma once
class CTimeUtil
{
public:
	// Convert SYSTEMTIME type into CString type
	static CString ToLDatetimeString(SYSTEMTIME t);
	static CString ToDatetimeString(SYSTEMTIME t);
	static CString ToDateString(SYSTEMTIME t);
	static CString ToTimeString(SYSTEMTIME t);
	static CString ToLTimeString(SYSTEMTIME t);

	// Convert CString type ino SYSTEMTIME type
	// yyyy-mm-dd hh:mm:ss SSS
	static SYSTEMTIME ToSystemTime(CString strTime);
	// Convert CTime type into SYSTEMTIME type
	static SYSTEMTIME ToSystemTime(CTime t);
	// Get the current system time
	static SYSTEMTIME GetCurrentTime();

	// Convert SYSTEMTIME type into CTime type
	static CTime ToCTime(SYSTEMTIME st);
};

