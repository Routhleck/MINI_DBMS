// INSERTRecord.cpp : 实现文件
//

#include "stdafx.h"
#include "DBMS.h"
#include "INSERTRecordDlg.h"
#include "afxdialogex.h"


// CINSERTRecord 对话框

IMPLEMENT_DYNAMIC(CINSERTRecordDlg, CDialogEx)

CINSERTRecordDlg::CINSERTRecordDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_INSERT, pParent)
{

}

CINSERTRecordDlg::~CINSERTRecordDlg()
{
}

void CINSERTRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA, m_lstData);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_editValue);
}


BEGIN_MESSAGE_MAP(CINSERTRecordDlg, CDialogEx)

	ON_NOTIFY(NM_CLICK, IDC_DATA, &CINSERTRecordDlg::OnClickListData)
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE, &CINSERTRecordDlg::OnKillfocusEditValue)
	ON_BN_CLICKED(IDOK, &CINSERTRecordDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CINSERTRecord 消息处理程序


BOOL CINSERTRecordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	DWORD dwStyle = ::GetWindowLong(m_lstData.m_hWnd, GWL_STYLE);
	dwStyle |= LVS_REPORT;// Set style
	::SetWindowLong(m_lstData.m_hWnd, GWL_STYLE, dwStyle);
	m_lstData.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	// List header
	m_lstData.InsertColumn(0, _T("Field Name"), LVCFMT_LEFT, 100);
	m_lstData.InsertColumn(1, _T("Field Type"), LVCFMT_LEFT, 100);
	m_lstData.InsertColumn(3, _T("Value"), LVCFMT_LEFT, 150);

	// Load the table information
	if (table != NULL)
	{
		// Get the numbet of field
		int nFieldNum = table->GetFieldNum();
		for (int i = 0; i < nFieldNum; i++)
		{
			// Get field
			CFieldEntity* pField = table->GetFieldAt(i);

			// Insert field name
			int nItem = m_lstData.InsertItem(i, pField->GetName());

			// Get the field type 
			int nDataType = pField->GetType();
			CString strTypeName = pField->GetTypeName(nDataType);
			// Insert the field type
			m_lstData.SetItemText(i, 1, strTypeName);
			m_lstData.SetItemData(i, nDataType);
		}
	}

	// Hide the edit control
	m_editValue.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CINSERTRecordDlg::OnClickListData(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	dwItem = pNMListView->iItem;  // The selected row
	nSubItem = pNMListView->iSubItem;  // The selected column
	DWORD dwCount = m_lstData.GetItemCount();  // The number of the data item in the list

											   // If the selected line number greater than 0, the column number greater than 1,
											   // and no more than the number of data items in the list, respond to the events
	if (dwItem >= 0 && nSubItem == 2 && dwItem <= dwCount)
	{
		CFieldEntity* pField = table->GetFieldAt(dwItem);

		// Get the size of the selected cell
		CRect rtEdit, rtClient;
		m_lstData.GetWindowRect(rtClient);
		ScreenToClient(rtClient);
		m_lstData.GetSubItemRect(dwItem, nSubItem, LVIR_LABEL, rtEdit);
		rtEdit.MoveToX(rtEdit.left + rtClient.left + 2);
		rtEdit.MoveToY(rtEdit.top + rtClient.top);
		rtEdit.bottom += 4;

		// Get the content of the selected cells
		CString strItemText = m_lstData.GetItemText(dwItem, nSubItem);
		int nLength = strItemText.GetLength();
		// Set the content of the edit control
		m_editValue.SetWindowText(strItemText);
		m_editValue.SetWindowPos(&wndTop, rtEdit.left, rtEdit.top, rtEdit.Width(), rtEdit.Height(), SWP_SHOWWINDOW);
		m_editValue.SetSel(nLength, nLength);
		m_editValue.SetFocus();
	}
}

void CINSERTRecordDlg::OnKillfocusEditValue()
{
	CString strValue;
	m_editValue.GetWindowText(strValue);

	m_lstData.SetItemText(dwItem, nSubItem, strValue);

	m_editValue.ShowWindow(SW_HIDE);
}

void CINSERTRecordDlg::SetTable(CTableEntity * pTable)
{
	table = pTable;
}

CRecordEntity CINSERTRecordDlg::GetRecord()
{
	return record;
}


void CINSERTRecordDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCount = m_lstData.GetItemCount();

	CString strField;
	CString strValue;
	CString strType;

	for (int i = 0; i < nCount; i++)
	{
		strField = m_lstData.GetItemText(i, 0);
		strType = m_lstData.GetItemText(i, 1);
		strValue = m_lstData.GetItemText(i, 2);
		
		if (!strValue.Trim().IsEmpty())
			record.Put(strField, strValue);
	}

	CDialogEx::OnOK();
}
