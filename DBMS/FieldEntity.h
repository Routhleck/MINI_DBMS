#pragma once
#include "DataStructure.h"
#include "CharUtil.h"
class CFieldEntity : public CObject{
private :
	CString		strName;
	int			nType;
	int			nParam;
	SYSTEMTIME	tLMTime;
	int			nIntegrities;
	CString		defaultVal;
	BOOL		isPrimaryKey;
public :
	CFieldEntity();
	CFieldEntity(CFieldEntity&);
	CFieldEntity(CString, int, int, int);
	~CFieldEntity();
	void SetField(Field fd);
	Field GetField();
	CString GetTypeName(int nDataType);
public :
	
	void SetName(CString name);
	void SetType(int type);
	void SetParam(int param);
	void SetLMTime(SYSTEMTIME time);
	void SetIntegerities(int integerites);
	void SetDefaultValue(CString val);
	void SetIsPrimaryKey(BOOL primaryKey);

	BOOL IsPrimaryKey();
	CString GetDefaultValue();
	CString GetName();
	int GetType();
	int GetParam();
	SYSTEMTIME GetLMTime();
	int GetIntegerities();
public:
	enum DataType
	{
		DT_INTEGER = 0,// Integer: 4byte  corresponding: int
		DT_BOOL = 1,// Boolean: 1byte, corresponding£ºbool
		DT_DOUBLE = 2,// Floating-point number: 2byte,  corresponding: double
		DT_VARCHAR = 3,// String: 256byte, corresponding: char[256]
		DT_DATETIME = 4// Data time: 16byte, corresponding: SYSTEMTIME type
	};
};
typedef CTypedPtrArray<CPtrArray, CFieldEntity*> FIELDARRAY;