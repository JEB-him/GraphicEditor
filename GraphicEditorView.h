
// GraphicEditorView.h: CGraphicEditorView 类的接口
//

#pragma once

enum OperationMode : unsigned int {
    OP_NONE             = 0b00000000,   // 缺省，点击空白区域等
    // Simple modes.
	OP_SCALE            = 0b00000001,   // 缩放某一图形/组合
	OP_MOVE             = 0b00000010,   // 移动某一个图形
	OP_SELECT           = 0b00000100,   // 选择某一图形/组合
    // 创建图形
	OP_CREATE           = 0b10000000,   // 创建图形
	OP_CREATE_RECTANGLE = 0b00001000,   // 创建一个矩形
	OP_CREATE_ELLIPSE   = 0b00010000,   // 创建一个椭圆
	OP_CREATE_TRIANGLE  = 0b00100000,   // 创建一个三角形
	OP_CREATE_LINE      = 0b01000000    // 创建一条直线
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
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void Set_Linecolor();
	afx_msg void Set_FIlledcolor();
	afx_msg void LineMode();
	afx_msg void TriangleMode();
	afx_msg void RectangleMode();
	afx_msg void EllipseMode();
	afx_msg void Set_LineCategory1();
	afx_msg void Set_LineCategory2();
};

#ifndef _DEBUG  // GraphicEditorView.cpp 中的调试版本
inline CGraphicEditorDoc* CGraphicEditorView::GetDocument() const
   { return reinterpret_cast<CGraphicEditorDoc*>(m_pDocument); }
#endif

