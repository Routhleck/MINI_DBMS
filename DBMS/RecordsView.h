#pragma once
#include "RecordEntity.h"	
#include "TableEntity.h"
#include <afxcview.h>

// CRecordsView view
/**********************************************
[ClassName]	CRecordsView
[Function]	Display record views, inherit from CListView
**********************************************/
class CRecordsView : public CListView
{
	DECLARE_DYNCREATE(CRecordsView)

protected:
	CRecordsView();           // protected constructor used by dynamic creation
	virtual ~CRecordsView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

protected:
	CListCtrl* m_pListCtrl;
	CTableEntity* m_pTable;
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	void AddRecord(CRecordEntity* pRecord);	// Insert record
public:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
};


