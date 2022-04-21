#pragma once


// CCREATETableDlg 对话框

class CCREATETableDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCREATETableDlg)

public:
	CCREATETableDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCREATETableDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CString strTableName;
public:
	CString GetTableName();
};
