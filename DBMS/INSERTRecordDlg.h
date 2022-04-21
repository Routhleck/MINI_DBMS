#pragma once
#include "afxcmn.h"
#include "TableEntity.h"
#include "RecordEntity.h"

// CINSERTRecord 对话框

class CINSERTRecordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CINSERTRecordDlg)

public:
	CINSERTRecordDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CINSERTRecordDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INSERT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstData;
	CEdit m_editValue;

private:
	CTableEntity* table;
	DWORD dwItem;
	int nSubItem;
	CRecordEntity record;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillfocusEditValue();
	void SetTable(CTableEntity* pTable);
	CRecordEntity GetRecord();
	afx_msg void OnBnClickedOk();
};
