#include "stdafx.h"
#include "DBTreeView.h"
#include "DBMSDoc.h"
#include "Global.h"
#include "MainFrm.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CDBTreeView, CTreeView)
CDBTreeView::CDBTreeView()
{
}

CDBTreeView::~CDBTreeView(){}

BEGIN_MESSAGE_MAP(CDBTreeView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CDBTreeView::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CDBTreeView::OnNMRClick)
END_MESSAGE_MAP()

#ifdef _DEBUG
void CDBTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CDBTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif

void CDBTreeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	CDBMSDoc* pDoc = (CDBMSDoc*)this->GetDocument();

	CString strError = pDoc->GetError();

	if (strError.GetLength() != 0)
	{
		AfxMessageBox(strError);

		pDoc->SetError(_T(""));
		return;
	}

	m_imageList.DeleteImageList();

	m_imageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 4);

	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_DATABASE));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TABLE));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_CHILD));

	pTreeCtrl = &this->GetTreeCtrl();

	// Get the style of the tree control
	DWORD dwStyle = ::GetWindowLong(pTreeCtrl->m_hWnd, GWL_STYLE);

	// Set the style of the tree control
	dwStyle |= TVS_HASBUTTONS	// Expand or collapse the child items
		| TVS_HASLINES		// Draw lines linked child items to their corresponding parent item
		| TVS_LINESATROOT;	// Draw lines linked child items to the root item
	::SetWindowLong(pTreeCtrl->m_hWnd, GWL_STYLE, dwStyle);


	pTreeCtrl->SetImageList(&m_imageList, TVSIL_NORMAL);

	pDoc->LoadDatabase();
	int count = pDoc->GetDBNum();
	for (int i = 0; i < count; i++) {
		CString strDBName = pDoc->GetDBAt(i)->GetName();

		HTREEITEM hRoot = pTreeCtrl->InsertItem(strDBName, 0, 0, NULL);

		pTreeCtrl->SetItemData(hRoot, MENU_DATABASE);
		pTreeCtrl->Expand(hRoot, TVE_EXPAND);

	}
}


void CDBTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pSender == NULL)
	{
		switch (lHint)
		{
		case UPDATE_CREATE_DATABASE: {
			CString strDBName = ((CDBEntity*)pHint)->GetName();

			HTREEITEM hRoot = pTreeCtrl->InsertItem(strDBName, 0, 0, NULL);
			pTreeCtrl->SetItemData(hRoot, MENU_DATABASE);

			pTreeCtrl->SelectItem(hRoot);
			break;
		}
		case UPDATE_OPEN_DATABASE: {
			pTreeCtrl->DeleteAllItems();
			CDBMSDoc* pDoc = (CDBMSDoc*)this->GetDocument();
			int k = pDoc->GetDBNum();
			for (int i = 0; i < k; i++) {

				CDBEntity *db = pDoc->GetDBAt(i);
				CString strDBName = db->GetName();

				pDoc->SetDBEntity(db);
				pDoc->LoadTables();

				HTREEITEM hRoot = pTreeCtrl->InsertItem(strDBName, 0, 0, NULL);

				int l = pDoc->GetTableNum();
				for (int j = 0; j < l; j++){
					CTableEntity* pTableEntity = pDoc->GetTBAt(j);
					AddTableNode(hRoot, pTableEntity);
				}

				pTreeCtrl->SetItemData(hRoot, MENU_DATABASE);
				pTreeCtrl->Expand(hRoot, TVE_EXPAND);
			}
			pTreeCtrl->SelectItem(pTreeCtrl->GetRootItem());
			break;
		}
		case UPDATE_RENAME_TABLE: {
			CDBMSDoc* pDoc = (CDBMSDoc*)this->GetDocument();

			pDoc->UpdateAllViews(NULL, UPDATE_OPEN_DATABASE, NULL);
			break;
		}
		case UPDATE_CREATE_TABLE: {
			CTableEntity* pTable = (CTableEntity*)pHint;
			
			HTREEITEM hSelectItem = pTreeCtrl->GetSelectedItem();
			HTREEITEM hParentItem = pTreeCtrl->GetParentItem(hSelectItem);
			HTREEITEM hTableNode;
			if (hParentItem == NULL)
				hTableNode = AddTableNode(hSelectItem, pTable);
			else 
				hTableNode = AddTableNode(hParentItem, pTable);
			pTreeCtrl->SelectItem(hTableNode);
			break;
		}
		case UPDATE_ADD_FIELD: {
			CFieldEntity* pField = (CFieldEntity*)pHint;
			CDBMSDoc* pDoc = (CDBMSDoc*)this->GetDocument();
			CTableEntity* pTable = pDoc->GetSelectedTB();
			HTREEITEM hSelectedItem = pTreeCtrl->GetSelectedItem();
			HTREEITEM hParentItem = pTreeCtrl->GetParentItem(hSelectedItem);
			HTREEITEM hChildItem = pTreeCtrl->GetChildItem(hSelectedItem);
			HTREEITEM hTBItem = NULL;
			if (pTreeCtrl->GetItemText(hParentItem).CompareNoCase(_T("Column")) == 0) {
				hTBItem = pTreeCtrl->GetParentItem(hParentItem);
			}
			else if (pTreeCtrl->GetItemText(hChildItem).CompareNoCase(_T("Column")) == 0) {
				hTBItem = hSelectedItem;
			}
			if (hTBItem != NULL) {
				HTREEITEM hFieldItem = AddFieldNode(pField, hTBItem);
				pTreeCtrl->SelectItem(hFieldItem);
			}
			break;
		}
		}
	}
}

HTREEITEM CDBTreeView::AddTableNode(HTREEITEM hRootNode, CTableEntity* pTable){
	if (hRootNode != NULL){

		HTREEITEM hTableNode = pTreeCtrl->InsertItem(pTable->GetName(), 1, 1, hRootNode);

		if (hTableNode != NULL){

			pTreeCtrl->SetItemData(hTableNode, MENU_TABLE);

			HTREEITEM hColNode = pTreeCtrl->InsertItem(_T("Column"), 2, 2, hTableNode);

			pTreeCtrl->SetItemData(hColNode, MENU_RCLICK);


			int nFieldNum = pTable->GetFieldNum();
			for (int i = 0; i < nFieldNum; i++)
			{
				CFieldEntity* pField = pTable->GetFieldAt(i);
				AddFieldNode(pField, hTableNode);
			}

			pTreeCtrl->Expand(hTableNode, TVE_EXPAND);

			pTreeCtrl->Expand(hRootNode, TVE_EXPAND);

			return hTableNode;
		}
	}
	return NULL;
}

HTREEITEM CDBTreeView::AddFieldNode(CFieldEntity* pField, HTREEITEM hTableItem){
	HTREEITEM hItem = pTreeCtrl->GetChildItem(hTableItem);

	if (hItem != NULL){

		do{
			if (pTreeCtrl->GetItemText(hItem).CompareNoCase(_T("Column")) == 0){
				break;
			}
		} while ((hItem = pTreeCtrl->GetNextSiblingItem(hItem)) != NULL);
	}

	HTREEITEM hFieldNode = pTreeCtrl->InsertItem(pField->GetName(), 1, 1, hItem);

	pTreeCtrl->SetItemData(hFieldNode, MENU_FIELD);

	return hFieldNode;
}

void CDBTreeView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CDBMSDoc* pDoc = (CDBMSDoc*)this->GetDocument();

	HTREEITEM hSelectedItem = pTreeCtrl->GetSelectedItem();

	if (hSelectedItem != NULL) {
		DWORD dwVal = pTreeCtrl->GetItemData(hSelectedItem);
		if (dwVal == MENU_DATABASE) {
			HTREEITEM hFindItem = hSelectedItem;
			int nIndex = 0;
			while ((hFindItem = pTreeCtrl->GetNextItem(hFindItem, TVGN_PREVIOUS)) != NULL) {
				nIndex++;
			}
			pDoc->SetDBEntity(pDoc->GetDBAt(nIndex));
			pDoc->LoadTables();

			pDoc->SetSelectedTB(NULL);
			CString strTitle;
			strTitle.Format(_T("Database(%s)"), pDoc->GetDBEntity()->GetName());
			pDoc->SetTitle(strTitle);

			((CMainFrame *)AfxGetMainWnd())->SwitchView(DEFAULT);
		}
		else if (dwVal == MENU_TABLE) {
			HTREEITEM hFindItem = hSelectedItem;
			HTREEITEM hParentItem = pTreeCtrl->GetParentItem(hFindItem);
			int i = 0;
			while ((hFindItem = pTreeCtrl->GetNextItem(hFindItem, TVGN_PREVIOUS)) != NULL) {
				i++;
			}
			int j = 0;
			while ((hParentItem = pTreeCtrl->GetNextItem(hParentItem, TVGN_PREVIOUS)) != NULL) {
				j++;
			}
			pDoc->SetDBEntity(pDoc->GetDBAt(j));
			pDoc->LoadTables();
			pDoc->SetSelectedTB(pDoc->GetTBAt(i));

			CString strTitle;
			strTitle.Format(_T("Database(%s).Tabe(%s)"), pDoc->GetDBEntity()->GetName(), pDoc->GetSelectedTB()->GetName());
			pDoc->SetTitle(strTitle);

			((CMainFrame *)AfxGetMainWnd())->SwitchView(TABLE);
			pDoc->UpdateAllViews(NULL, UPDATE_TABLE_VIEW, pDoc->GetSelectedTB());
		}
		else if (dwVal == MENU_FIELD) {
			HTREEITEM hColumnItem = pTreeCtrl->GetParentItem(hSelectedItem);
			if (pTreeCtrl->GetItemText(hColumnItem).CompareNoCase(_T("Column")) == 0) {
				HTREEITEM hTBItem = pTreeCtrl->GetParentItem(hColumnItem);
				HTREEITEM hDBItem = pTreeCtrl->GetParentItem(hTBItem);
				int i = 0;
				while ((hTBItem = pTreeCtrl->GetNextItem(hTBItem, TVGN_PREVIOUS)) != NULL) {
					i++;
				}
				int j = 0;
				while ((hDBItem = pTreeCtrl->GetNextItem(hDBItem, TVGN_PREVIOUS)) != NULL) {
					j++;
				}
				pDoc->SetDBEntity(pDoc->GetDBAt(j));
				pDoc->LoadTables();
				pDoc->SetSelectedTB(pDoc->GetTBAt(i));
				CTableEntity* pTable = pDoc->GetSelectedTB();

				if (pTable != NULL)
				{
					pDoc->LoadRecord();

					CString strError = pDoc->GetError();
					if (strError.GetLength() > 0)
					{
						AfxMessageBox(strError);
						pDoc->SetError(_T(""));
						return;
					}

					((CMainFrame *)AfxGetMainWnd())->SwitchView(RECORD);
					pDoc->UpdateAllViews(NULL, UPDATE_RECORD_VIEW, pTable);
				}
			}
		}
		*pResult = 0;
	}
}

void CDBTreeView::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult) {
	CPoint point;
	GetCursorPos(&point);

	pTreeCtrl->ScreenToClient(&point);

	UINT nFlag = TVHT_ONITEM;
	HTREEITEM hSelectedItem = pTreeCtrl->HitTest(point, &nFlag);
	CString temp = pTreeCtrl->GetItemText(hSelectedItem);

	if (NULL != hSelectedItem) {
		DWORD dwVal = pTreeCtrl->GetItemData(hSelectedItem);
		if (dwVal != MENU_RCLICK) {
			pTreeCtrl->ClientToScreen(&point);

			CMenu* pMenu = this->GetParentFrame()->GetMenu()->GetSubMenu(dwVal);

			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, AfxGetMainWnd());
		}

	}
	*pResult = 0;
}