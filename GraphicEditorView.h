
// GraphicEditorView.h: CGraphicEditorView 类的接口
//

#pragma once

enum OperationMode : unsigned int {
    OP_NONE   = 0,         // 缺省，点击空白区域等
    // Simple modes.
	OP_SCALE  = 0b1000001, // 缩放某一图形/组合
	OP_MOVE   = 0b1000010, // 移动某一个图形
	OP_SELECT = 0b1000100, // 选择某一图形/组合
    // 创建图形
	OP_CREATE           = 0b10000000,
	OP_CREATE_RECTANGLE = 0b10000001,   // 创建一个矩形
	OP_CREATE_ELLIPSE   = 0b10000010,   // 创建一个椭圆
	OP_CREATE_TRIANGLE  = 0b10000100    // 创建一个三角形
};

class CGraphicEditorView : public CView
{
protected: // 仅从序列化创建
	CGraphicEditorView() noexcept;
	DECLARE_DYNCREATE(CGraphicEditorView)

// 特性
public:
	CGraphicEditorDoc* GetDocument() const;

// 操作
public: 

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CGraphicEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    unsigned int m_opMode = OperationMode::OP_NONE;

    // Drawing properties.
    Color       m_filled_color = RGB(255, 255, 255); // Default: White.
    Color       m_border_color = RGB(0, 0, 0);       // Default: Black.
    BorderWidth m_border_width = 2;
    BorderStyle m_border_style = PS_SOLID;

    // Opration object
    CShape* selected_shape;

// 内部函数
protected:
    void CreateShape(const int& x, const int& y);

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // GraphicEditorView.cpp 中的调试版本
inline CGraphicEditorDoc* CGraphicEditorView::GetDocument() const
   { return reinterpret_cast<CGraphicEditorDoc*>(m_pDocument); }
#endif

