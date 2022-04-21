#pragma once
#include "TableEntity.h"	
#include "FieldEntity.h"
#include <afxcview.h>

class CTableView : public CListView
{
	DECLARE_DYNCREATE(CTableView)

protected:
	CTableView();           // protected constructor used by dynamic creation
	virtual ~CTableView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	CListCtrl* m_pListCtrl;	// List control
	CTableEntity* m_pTable;	// Pointer to the table entity

							// Add field
	void AddField(CFieldEntity* pField);

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
};


