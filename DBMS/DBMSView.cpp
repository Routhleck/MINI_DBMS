
// DBMSView.cpp : CDBMSView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "DBMS.h"
#endif

#include "DBMSDoc.h"
#include "DBMSView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDBMSView

IMPLEMENT_DYNCREATE(CDBMSView, CView)

BEGIN_MESSAGE_MAP(CDBMSView, CView)
END_MESSAGE_MAP()

// CDBMSView 构造/析构

CDBMSView::CDBMSView()
{
	// TODO: 在此处添加构造代码

}

CDBMSView::~CDBMSView()
{
}

BOOL CDBMSView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CDBMSView 绘制

void CDBMSView::OnDraw(CDC* /*pDC*/)
{
	CDBMSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CDBMSView 诊断

#ifdef _DEBUG
void CDBMSView::AssertValid() const
{
	CView::AssertValid();
}

void CDBMSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDBMSDoc* CDBMSView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDBMSDoc)));
	return (CDBMSDoc*)m_pDocument;
}
#endif //_DEBUG


// CDBMSView 消息处理程序
