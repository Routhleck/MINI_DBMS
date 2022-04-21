// ALTERTableDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DBMS.h"
#include "RENAMETableDlg.h"
#include "afxdialogex.h"


// CALTERTableDlg 对话框

IMPLEMENT_DYNAMIC(CRENAMETableDlg, CDialogEx)

CRENAMETableDlg::CRENAMETableDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RENAME_TABLE, pParent)
	, newTableName(_T(""))
	, databaseName(_T(""))
{

}

CRENAMETableDlg::~CRENAMETableDlg()
{
}

void CRENAMETableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NEW_TABLE_NAME, newTableName);
	DDX_Text(pDX, IDC_OLD_DATABASE_NAME, databaseName);
	DDX_Text(pDX, IDC_OLD_TABLE_NAME, oldName);
}

CString CRENAMETableDlg::GetNewTableName(){
	return newTableName;
}

void CRENAMETableDlg::SetDatabaseName(CString name)
{
	this->databaseName = name;
}

void CRENAMETableDlg::SetOldTableName(CString oldName)
{
	this->oldName = oldName;
}


BEGIN_MESSAGE_MAP(CRENAMETableDlg, CDialogEx)
END_MESSAGE_MAP()


// CALTERTableDlg 消息处理程序
