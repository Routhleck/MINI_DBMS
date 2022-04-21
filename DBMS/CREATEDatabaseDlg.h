#pragma once


// CCREATEDatabaseDlg 对话框

class CCREATEDatabaseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCREATEDatabaseDlg)

public:
	CCREATEDatabaseDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCREATEDatabaseDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_DATABASE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CString strDatabaseName;
public:
	CString GetDatabaseName();
};
