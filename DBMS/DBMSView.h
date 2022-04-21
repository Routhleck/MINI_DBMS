
// DBMSView.h : CDBMSView 类的接口
//

#pragma once

class CDBMSDoc;
class CDBMSView : public CView
{
protected: // 仅从序列化创建
	CDBMSView();
	DECLARE_DYNCREATE(CDBMSView)

// 特性
public:
	CDBMSDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CDBMSView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // DBMSView.cpp 中的调试版本
inline CDBMSDoc* CDBMSView::GetDocument() const
   { return reinterpret_cast<CDBMSDoc*>(m_pDocument); }
#endif

