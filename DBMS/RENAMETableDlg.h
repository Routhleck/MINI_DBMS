#pragma once


// CREANAMETableDlg 对话框

class CRENAMETableDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRENAMETableDlg)

public:
	CRENAMETableDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRENAMETableDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RENAME_TABLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CString newTableName;
	CString oldName;
public:
	CString GetNewTableName();
	void SetDatabaseName(CString name);
	void SetOldTableName(CString oldName);
	CString databaseName;
};
