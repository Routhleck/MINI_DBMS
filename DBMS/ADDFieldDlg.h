#pragma once

#include "FieldEntity.h"
// CADDFieldDlg 对话框

class CADDFieldDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CADDFieldDlg)

public:
	CADDFieldDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CADDFieldDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_FIELD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CString fieldName;
	CString defaultVal;
	int datatype;
	int param;
	CString type;
	BOOL isNotNull;
	BOOL isPrimaryKey;

	CComboBox m_cbType;

public:
	CString GetFieldName();
	CString GetDefaultVal();
	int GetDatatype();
	int GetParam();

	BOOL IsNotNull();
	BOOL IsPrimaryKey();

	void SetPrimaryKey(BOOL);

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
