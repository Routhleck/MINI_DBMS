#pragma once
#include <afxcview.h>
#include "TableEntity.h"
#include "FieldEntity.h"

class CDBTreeView : public CTreeView {
	DECLARE_DYNCREATE(CDBTreeView);
private:
	CTreeCtrl* pTreeCtrl;
	CImageList m_imageList;
protected :
	CDBTreeView();
	virtual ~CDBTreeView();
public :
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	HTREEITEM AddTableNode(HTREEITEM hRootNode, CTableEntity * pTable);
	HTREEITEM AddFieldNode(CFieldEntity * pField, HTREEITEM hTableItem);

	afx_msg void OnTvnSelchanged(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMRClick(NMHDR * pNMHDR, LRESULT * pResult);
};

