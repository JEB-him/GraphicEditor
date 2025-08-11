
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
    ON_WM_ERASEBKGND()   // 添加背景擦除处理
    ON_WM_SIZE()         // 添加窗口大小改变处理
    ON_COMMAND(ID_32771, &CGraphicEditorView::Line)
    ON_COMMAND(ID_32772, &CGraphicEditorView::Triangle)
    ON_COMMAND(ID_32773, &CGraphicEditorView::Rectangle)
    ON_COMMAND(ID_32774, &CGraphicEditorView::Ellipse)
    ON_COMMAND(ID_32775, &CGraphicEditorView::LineColor)
    ON_COMMAND(ID_32776, &CGraphicEditorView::FilledColor)
    ON_COMMAND(ID_32777, &CGraphicEditorView::LineType1)
    ON_COMMAND(ID_32778, &CGraphicEditorView::LineType2)
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
    CRect rect;
    GetClientRect(&rect);

    // 当窗口最小化时跳过绘制
    if (rect.IsRectEmpty())
        return;

    // 双缓冲绘图
    CDC memDC;
    CBitmap memBitmap;
    memDC.CreateCompatibleDC(pDC);
    memBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

    try {
        // 使用GDI+双缓冲
        Gdiplus::Graphics graphics(memDC.GetSafeHdc());
        Gdiplus::Bitmap bufferBmp(rect.Width(), rect.Height(), PixelFormat32bppARGB);
        Gdiplus::Graphics bufferGraphics(&bufferBmp);

        // 高质量渲染设置
        bufferGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        bufferGraphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

        // 绘制白色背景
        bufferGraphics.Clear(Gdiplus::Color(255, 255, 255));

        // 绘制文档内容
        ASSERT_VALID(pDoc);
        if (pDoc) {
            const CObArray& shapes = pDoc->GetShapes();
            for (int i = 0; i < shapes.GetCount(); i++) {
                if (CShape* pShape = dynamic_cast<CShape*>(shapes[i])) {
                    pShape->draw(bufferGraphics);
                }
            }
        }

        // 绘制到内存DC
        graphics.DrawImage(&bufferBmp, 0, 0);
    }
    catch (...) {
        // 异常处理
    }

    // 将最终图像绘制到屏幕
    pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

    // 清理资源
    memDC.SelectObject(pOldBitmap);
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
    if (m_opMode & OperationMode::OP_CREATE) {
        CreateShape(point.x, point.y);
        // Switch to SCALE mode afetr shape creation.
        m_opMode |= OperationMode::OP_SCALE;
        Invalidate();
        return;
    } else if (m_opMode & OperationMode::OP_SELECT) {
        // TODO: Check if click the empty area.
    } else if (m_opMode == 0) {
        // TODO: Check if click any combination or shape.
    } else {
      throw std::runtime_error("Unknown Operation Mode.");
    }
}
afx_msg void CGraphicEditorView::OnLButtonUp(UINT nFlags, CPoint point) {
    if (m_opMode & OperationMode::OP_SELECT) {
        // SELECT mode
        if (!(m_opMode & OperationMode::OP_CREATE)) {
            m_opMode = OperationMode::OP_SELECT;
        }
        else {
            m_opMode ^= OperationMode::OP_SCALE;
        }
        m_opMode = OperationMode::OP_SELECT;
        selected_shape->setMode(EditMode::SELECT);
        Invalidate();
    } else {

    }

}

afx_msg void CGraphicEditorView::OnMouseMove(UINT nFlags, CPoint point) {
    if (m_opMode & OperationMode::OP_SCALE) {
        //// 获取原始边界矩形
        //CRect oldRect = selected_shape->getBoundingRect();

        //// 更新形状
        //selected_shape->scale(this, point.x, point.y);

        //// 获取新边界矩形
        //CRect newRect = selected_shape->getBoundingRect();

        //// 计算需要重绘的区域
        //CRect updateRect;
        //updateRect.UnionRect(&oldRect, &newRect);  // 合并新旧区域

        //// 添加边界余量（避免抗锯齿导致的残留）
        //updateRect.InflateRect(5, 5);
        //// 只重绘受影响区域
        selected_shape->scale(this, point.x, point.y);
        Invalidate();
        //InvalidateRect(updateRect);  // 关键调用
    }
}

BOOL CGraphicEditorView::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;  // 禁用默认背景擦除
}

void CGraphicEditorView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    Invalidate();  // 触发重绘
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

    if (m_opMode & OperationMode::OP_CREATE_RECTANGLE) {
        CRectangle* pRect = new CRectangle(
            CShape::z_max,
            x,
            y,
            m_filled_color,
            m_border_width,
            m_border_color,
            m_border_style
        );
        bool isSussussed = pDoc->AddShape(pRect);
        if (!isSussussed) {
            delete pRect; // 添加失败时释放内存
            throw std::runtime_error("Failed to add rectangle.");
        }
        // Add SELECT Mode (CREATE Mode has been initialized in class)
        pRect->addMode(EditMode::SELECT);
        selected_shape = pRect;
        // Increase z_max
        ++CShape::z_max;
    }
}

// CGraphicEditorView 消息处理程序

void CGraphicEditorView::Line()
{
    m_opMode = OperationMode::OP_CREATE_LINE | OperationMode::OP_SELECT | OperationMode::OP_CREATE;
}

void CGraphicEditorView::Triangle()
{
    m_opMode = OperationMode::OP_CREATE_TRIANGLE | OperationMode::OP_SELECT | OperationMode::OP_CREATE;
}

void CGraphicEditorView::Rectangle()
{
    m_opMode = OperationMode::OP_CREATE_RECTANGLE | OperationMode::OP_SELECT | OperationMode::OP_CREATE;
}

void CGraphicEditorView::Ellipse()
{
    m_opMode = OperationMode::OP_CREATE_ELLIPSE | OperationMode::OP_SELECT | OperationMode::OP_CREATE;
}

void CGraphicEditorView::LineColor()
{
    CColorDialog dlg(m_border_color); // 默认选中当前颜色
    if (dlg.DoModal() == IDOK)    // 用户点击"确定"
    {
        m_border_color = dlg.GetColor(); // 更新颜色
        Invalidate(); // 重绘视图
    }
}

void CGraphicEditorView::FilledColor()
{
    CColorDialog dlg(m_filled_color); // 默认选中当前颜色
    if (dlg.DoModal() == IDOK)    // 用户点击"确定"
    {
        m_filled_color = dlg.GetColor(); // 更新颜色
        Invalidate(); // 重绘视图
    }
}

void CGraphicEditorView::LineType1()
{
    m_border_style = PS_SOLID;
}

void CGraphicEditorView::LineType2()
{
    m_border_style = PS_DASH;
}
