
// GraphicEditorView.cpp: CGraphicEditorView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "GraphicEditor.h"
#endif

#include "GraphicEditorDoc.h"
#include "GraphicEditorView.h"

// Shapes
#include "Shapes/Shape.h"
#include "Shapes/Rectangle.h"

#include <stdexcept>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGraphicEditorView

IMPLEMENT_DYNCREATE(CGraphicEditorView, CView)

BEGIN_MESSAGE_MAP(CGraphicEditorView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CGraphicEditorView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
    ON_WM_LBUTTONDOWN()  // 映射左键按下消息
    ON_WM_MOUSEMOVE()    // 映射鼠标移动消息
	ON_WM_LBUTTONUP()    // 映射左键释放消息
	ON_WM_RBUTTONUP()    // 映射右键释放消息
END_MESSAGE_MAP()

// CGraphicEditorView 构造/析构

CGraphicEditorView::CGraphicEditorView() noexcept
{
	// TODO: 在此处添加构造代码

}

CGraphicEditorView::~CGraphicEditorView()
{
}

BOOL CGraphicEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CGraphicEditorView 绘图


void CGraphicEditorView::OnDraw(CDC* pDC)
{
    CGraphicEditorDoc* pDoc = GetDocument();
    CRect   rect;
    // Get the size of view.
    GetClientRect(&rect);

    // Screen size
    int width  = rect.Width();
    int height = rect.Height();

    // 使用 GDI+ 双缓冲
    Gdiplus::Graphics graphics(pDC->GetSafeHdc());

    // 创建内存位图（32位带alpha通道）
    Gdiplus::Bitmap bufferBmp(width, height, PixelFormat32bppARGB);
    Gdiplus::Graphics bufferGraphics(&bufferBmp);

    // 设置高质量渲染
    bufferGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    bufferGraphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

    // 绘制白色背景
    bufferGraphics.Clear(Gdiplus::Color(255, 255, 255));

    /*========= Test ========*/
    CRectangle rect1 {1, 0, 0, RGB(255, 0, 0), 2, RGB(0, 255, 0), PS_SOLID};
    CRectangle rect2 {1, 10, 20, RGB(0, 255, 0), 2, RGB(0, 0, 255), PS_SOLID};
    rect1.setMode(EditMode::CREATE);
    rect1.scale(this, 50, 60);
    rect2.setMode(EditMode::CREATE);
    rect2.scale(this, 70, 40);

    // Drawing
    rect1.draw(bufferGraphics);
    rect2.draw(bufferGraphics);

    // 将双缓冲内容绘制到屏幕
    graphics.DrawImage(&bufferBmp, 0, 0);
    return;

    /*======= Test End ======*/
    ASSERT_VALID(pDoc);
    if (!pDoc)
      return;

    // 绘制所有形状
    float last_z = -1;
    const CObArray& shapes = pDoc->GetShapes();
    for (int i = 0; i < shapes.GetCount(); i++) {
        CShape* pShape = static_cast<CShape*>(shapes[i]);
        if (pShape) {
            if (last_z ==  pShape->getZ()) {
                throw std::runtime_error("Same z-index found!");
            }
            pShape->draw(bufferGraphics);
            last_z = pShape->getZ();
        } else {
            throw std::runtime_error("Null value Cshape pointer found!");
        }
    }
}


// CGraphicEditorView 打印


void CGraphicEditorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CGraphicEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CGraphicEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CGraphicEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CGraphicEditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CGraphicEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

afx_msg void CGraphicEditorView::OnLButtonDown(UINT nFlags, CPoint point) {
    // 创建图形
    // Debug statement
    m_opMode = OperationMode::OP_CREATE_RECTANGLE | OperationMode::OP_SELECT;
    // End Debug
    if (m_opMode & OperationMode::OP_CREATE) {
        CreateShape(point.x, point.y);
        // Switch to SCALE mode afetr shape creation.
        m_opMode = OperationMode::OP_SCALE;
        Invalidate();
        return;
    } else if (m_opMode & OperationMode::OP_SELECT) {
        // TODO: Check if click the empty area.
    } else {
      throw std::runtime_error("Unknown Operation Mode.");
    }
}
afx_msg void CGraphicEditorView::OnLButtonUp(UINT nFlags, CPoint point) {
    if (m_opMode & OperationMode::OP_SELECT) {
        // SELECT mode
        m_opMode = OperationMode::OP_SELECT;
        selected_shape->setMode(EditMode::SELECT);
        Invalidate();
    } else {

    }

}

afx_msg void CGraphicEditorView::OnMouseMove(UINT nFlags, CPoint point) {
    if (m_opMode & OperationMode::OP_SCALE) {
        selected_shape->scale(this, point.x, point.y);
    }
}


// CGraphicEditorView 诊断

#ifdef _DEBUG
void CGraphicEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CGraphicEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGraphicEditorDoc* CGraphicEditorView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGraphicEditorDoc)));
	return (CGraphicEditorDoc*)m_pDocument;
}
#endif //_DEBUG

void CGraphicEditorView::CreateShape(const int& x, const int& y) {
    // 获取文档指针
    CGraphicEditorDoc* pDoc = GetDocument();

    if (!pDoc) {
        // TODO: Add error handle.
        return;
    }

    switch (m_opMode) {
        case OperationMode::OP_CREATE_RECTANGLE: {
            CRectangle* pRect = new CRectangle(
                CShape::z_max,
                x,
                y,
                m_filled_color,
                m_border_width,
                m_border_color,
                m_border_style
            );
            bool isSussussed = pDoc->AddShape(PRect);
            if (!isSussussed) {
                throw std::runtime_error("Failed to add rectangle.");
            }
            // Add SELECT Mode (CREATE Mode has been initialized in class)
            pRect->addMode(EditMode::SELECT);
            selected_shape = pRect;
            // Increase z_max
            ++CShape::z_max;
            break;
        } default:
            return;
    }
}

// CGraphicEditorView 消息处理程序
