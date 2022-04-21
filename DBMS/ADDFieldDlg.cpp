// ADDFieldDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DBMS.h"
#include "ADDFieldDlg.h"
#include "afxdialogex.h"


// CADDFieldDlg 对话框

IMPLEMENT_DYNAMIC(CADDFieldDlg, CDialogEx)

CADDFieldDlg::CADDFieldDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADD_FIELD, pParent)
	, fieldName(_T(""))
	, defaultVal(_T(""))
	, isNotNull(false)
	, isPrimaryKey(false)
{
	param = 0;
	datatype = 0;
	type = _T("");
}

CADDFieldDlg::~CADDFieldDlg()
{
}

void CADDFieldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FIELD_NAME, fieldName);
	DDX_Text(pDX, IDC_DEFAULT, defaultVal);
	DDX_Check(pDX, IDC_NOT_NULL, isNotNull);
	DDX_Check(pDX, IDC_PRIMARY_KEY, isPrimaryKey);
	DDX_Control(pDX, IDC_DATA_TYPE, m_cbType);
	DDX_CBString(pDX, IDC_DATA_TYPE, type);
}

CString CADDFieldDlg::GetFieldName()
{
	return fieldName;
}

CString CADDFieldDlg::GetDefaultVal()
{
	return defaultVal;
}

int CADDFieldDlg::GetDatatype()
{
	return datatype;
}

int CADDFieldDlg::GetParam()
{
	return param;
}

BOOL CADDFieldDlg::IsNotNull()
{
	return isNotNull;
}

BOOL CADDFieldDlg::IsPrimaryKey()
{
	return isPrimaryKey;
}

void CADDFieldDlg::SetPrimaryKey(BOOL torf)
{
	this->isPrimaryKey = torf;
}


BEGIN_MESSAGE_MAP(CADDFieldDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CADDFieldDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CADDFieldDlg 消息处理程序




void CADDFieldDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	fieldName.Trim();
	if (fieldName.IsEmpty()) {
		AfxMessageBox(_T("Field name can not be empty!"));

		return;
	}
	else {

		if (type.CollateNoCase(_T("INT")) == 0)
		{
			datatype = CFieldEntity::DT_INTEGER;
		}
		else if (type.CollateNoCase(_T("BOOL")) == 0)
		{
			datatype = CFieldEntity::DT_BOOL;
		}
		else if (type.CollateNoCase(_T("DOUBLE")) == 0)
		{
			datatype = CFieldEntity::DT_DOUBLE;
		}
		else if (type.CollateNoCase(_T("VARCHAR")) == 0)
		{
			datatype = CFieldEntity::DT_VARCHAR;
			param = 256;
		}
		else if (type.CollateNoCase(_T("DATATIME")) == 0)
		{
			datatype = CFieldEntity::DT_DATETIME;
		}
	}
	CDialogEx::OnOK();
}


BOOL CADDFieldDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_cbType.InsertString(0, _T("INT"));
	m_cbType.InsertString(1, _T("VARCHAR"));
	m_cbType.InsertString(2, _T("BOOL"));
	m_cbType.InsertString(3, _T("DOUBLE"));
	m_cbType.InsertString(4, _T("DATATIME"));
	m_cbType.SetCurSel(0);
	UpdateData();
	((CButton *)GetDlgItem(IDC_PRIMARY_KEY))->SetCheck(isPrimaryKey);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
