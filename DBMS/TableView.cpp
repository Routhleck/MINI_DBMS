// TableView.cpp : implementation file
//

#include "stdafx.h"
#include "DBMS.h"
#include "TableView.h"

#include "Global.h"
#include "DBMSDoc.h"


// CTableView

IMPLEMENT_DYNCREATE(CTableView, CListView)

CTableView::CTableView()
{
	m_pListCtrl = NULL;
}

CTableView::~CTableView()
{
}

BEGIN_MESSAGE_MAP(CTableView, CListView)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CTableView::OnNMRClick)
END_MESSAGE_MAP()


// CTableView diagnostics

#ifdef _DEBUG
void CTableView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CTableView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTableView message handlers

void CTableView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// Get list control
	m_pListCtrl = &this->GetListCtrl();

	// Get the default style of list control
	DWORD dwStyle = ::GetWindowLong(m_pListCtrl->m_hWnd, GWL_STYLE);

	// Set the style of the list control
	dwStyle |= LVS_REPORT;	// Report style
	::SetWindowLong(m_pListCtrl->m_hWnd, GWL_STYLE, dwStyle);

	// Set extended style
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	// Initialize the column of the list control
	m_pListCtrl->InsertColumn(0, _T("Column"), LVCFMT_CENTER, 100);
	m_pListCtrl->InsertColumn(1, _T("Data Type"), LVCFMT_CENTER, 100);
	m_pListCtrl->InsertColumn(2, _T("Default Value"), LVCFMT_CENTER, 200);
}

void CTableView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (pSender == NULL)
	{
		// Get the object of the document
		CDBMSDoc* pDoc = (CDBMSDoc*)this->GetDocument();
		switch (lHint)
		{
		case UPDATE_CREATE_TABLE:
		{
			m_pTable = (CTableEntity*)pHint;
		}
		break;
		case UPDATE_TABLE_VIEW:
		{
			m_pTable = (CTableEntity*)pHint;

			int nFieldNum = m_pTable->GetFieldNum();
			for (int i = 0; i < nFieldNum; i++)
			{
				AddField(m_pTable->GetFieldAt(i));
			}
		}
		break;
		case UPDATE_ADD_FIELD:
		{
			if (m_pTable == pDoc->GetSelectedTB())
			{
				CFieldEntity* pField = (CFieldEntity*)pHint;
				AddField(pField);
			}
		}
		break;
		default:
			break;
		}
	}
}



void CTableView::AddField(CFieldEntity* pField)
{
	// Get the number of the row int the list
	int nCount = m_pListCtrl->GetItemCount();

	// Insert data into the list
	m_pListCtrl->InsertItem(nCount, pField->GetName());	// Field name

	int nDataType = pField->GetType();				// Type
	CString nTypeName = pField->GetTypeName(nDataType);
	m_pListCtrl->SetItemText(nCount, 1, nTypeName);

	m_pListCtrl->SetItemText(nCount, 2, pField->GetDefaultValue());
}

void CTableView::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// Convert NMHDR* type into NM_LISTVIEW* type
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// Get the row number and column number of the selected item
	DWORD dwRow = pNMListView->iItem;  // The selected row
	int nCol = pNMListView->iSubItem;  // The selected column

									   // Get the cursor coordinates
	CPoint point;
	GetCursorPos(&point);

	// Load field menu resource to the CMenu object
	CMenu* pMenu = this->GetParentFrame()->GetMenu()->GetSubMenu(MENU_FIELD);

	// Display the menu in the position of the cursor clicked
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, AfxGetMainWnd());

	*pResult = 0;
}
