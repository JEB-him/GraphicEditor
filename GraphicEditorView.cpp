
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
#include "Shapes/Triangle.h"
#include "Shapes/Line.h"
#include "Shapes/Ellipse.h"
#include "Shapes/Combination.h"

#include <stdexcept>
#include <typeinfo>

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
    ON_WM_KEYDOWN()      //按键消息映射
    ON_COMMAND(ID_32773, &CGraphicEditorView::Set_Linecolor)
    ON_COMMAND(ID_32774, &CGraphicEditorView::Set_FIlledcolor)
    ON_COMMAND(ID_32775, &CGraphicEditorView::LineMode)
    ON_COMMAND(ID_32776, &CGraphicEditorView::TriangleMode)
    ON_COMMAND(ID_32777, &CGraphicEditorView::RectangleMode)
    ON_COMMAND(ID_32778, &CGraphicEditorView::EllipseMode)
    ON_COMMAND(ID_32780, &CGraphicEditorView::Set_LineCategory1)
    ON_COMMAND(ID_32781, &CGraphicEditorView::Set_LineCategory2)
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
            const CObArray& combinations = pDoc->GetCombinations();
            for (int i = 0; i < combinations.GetCount(); i++) {
                if (CShape* pShape = dynamic_cast<CShape*>(combinations[i])) {
                    pShape->draw(bufferGraphics);
        }
            }
        }

        DrawPoints(bufferGraphics);

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
    if (m_opMode & OperationMode::OP_CREATE_TRIANGLE) {     // 创建三角形 
        if (m_pn == 0 && selected_shape != nullptr) {
            selected_shape->setMode(0);
            ClearSelected();
        }
        m_points[m_pn].X   = point.x;
        m_points[m_pn++].Y = point.y;
        if (m_pn == 3) CreateShape();
        Invalidate();
    } else if (m_opMode & OperationMode::OP_CREATE) {       // 创建图形
        CreateShape(point.x, point.y);
        // Switch to SCALE mode afetr shape creation.
        m_opMode |= OperationMode::OP_SCALE;
        Invalidate();
        return;
    } else if (m_opMode == OperationMode::OP_SELECT) {       // 选择模式
        if (selected_shape == nullptr) throw std::runtime_error("Invalid opMode.");
        if (GetKeyState(VK_CONTROL) < 0) {
            CtrlClick(point);
            Invalidate();
            return;
        }
        if (selected.GetCount() > 1) {
            ClearSelected();
            m_opMode = NONE;
            OnLButtonDown(nFlags, point);
        }
        int back_code = selected_shape->inShape(point.x,point.y);
        switch(back_code) {
            case 0:
                m_opMode = OperationMode::OP_SCALE | OperationMode::OP_SELECT;
                selected_shape->setMode(EditMode::SCALE | EditMode::SELECT);
                break;
            case 1: {
                m_opMode = OperationMode::OP_MOVE | OperationMode::OP_SELECT;
                selected_shape->setMode(EditMode::MOVE | EditMode::SELECT);
                CRect rect = selected_shape->getRect();
                horizon = rect.left - point.x;
                vertical = rect.top - point.y;
                break;
            }

            case 2:
                break;
            case -1:
                m_opMode = OperationMode::OP_NONE;
                ClearSelected();
                Invalidate();
                break;
            default:
                throw std::runtime_error("Unknown Back Code.");
        }
    } else if (m_opMode == 0) {                             // NONE
        if (selected_shape) throw std::runtime_error("Invalid opMode.");
        CGraphicEditorDoc* pDoc = GetDocument();
        if (pDoc) {
            const CObArray& shapes = pDoc->GetShapes();
            for (int i = shapes.GetCount() - 1; i >= 0; i--) {
                if (CShape* pShape = dynamic_cast<CShape*>(shapes[i])) {
                    int back_code = pShape->inShape(point.x, point.y);
                    if (back_code == 2) {
                        pShape = pShape->getFather();
                        AddSelected(pShape);
                        m_opMode = OperationMode::OP_SELECT;
                        Invalidate();
                        break;
                    }
                }
            }
        }
        return;
    } else {
      throw std::runtime_error("Unknown Operation Mode.");
    }
}
afx_msg void CGraphicEditorView::OnLButtonUp(UINT nFlags, CPoint point) {
    if (m_opMode & OperationMode::OP_CREATE) {
        if (m_opMode & OperationMode::OP_SCALE)
            m_opMode ^= OperationMode::OP_SCALE;
        if (selected_shape != nullptr) selected_shape->setMode(EditMode::SELECT);
        AddSelected(selected_shape);
        Invalidate();
    } else if (m_opMode & OperationMode::OP_SCALE) {
        m_opMode = OperationMode::OP_SELECT;
        if (selected_shape != nullptr) {
            selected_shape->setMode(EditMode::SELECT);
            if (typeid(*selected_shape) == typeid(CTriangle)) {
                CTriangle* pTriangle = dynamic_cast<CTriangle*>(selected_shape);
                pTriangle->updateRect();
            }
        }
        ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        Invalidate();
    } else if (m_opMode & OperationMode::OP_MOVE) {
        m_opMode = OperationMode::OP_SELECT;
        if(selected_shape != nullptr) selected_shape->setMode(EditMode::SELECT);
        ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        Invalidate();
    }

}

afx_msg void CGraphicEditorView::OnMouseMove(UINT nFlags, CPoint point) {
    if (m_opMode & OperationMode::OP_SCALE) {
        selected_shape->scale(this, point.x, point.y);
        Invalidate();
        //InvalidateRect(updateRect);  // 关键调用
    } else if (m_opMode & OperationMode::OP_CREATE_TRIANGLE) {
        m_points[m_pn].X = point.x;
        m_points[m_pn].Y = point.y;
        Invalidate();
    } else if (m_opMode & OperationMode::OP_CREATE) {
        return;
    } else if (m_opMode & OperationMode::OP_MOVE) {
        int x = point.x + horizon;
        int y = point.y + vertical;
        selected_shape->move(this, x, y);
        Invalidate();
    } else if (m_opMode == OperationMode::OP_SELECT) {
        int back_code = selected_shape->inShape(point.x, point.y);
        switch(back_code) {
            case 0:
                ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));
                break;
            case 1:
                ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
                break;
            case 2:
            case -1:
                ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
                break;
            default:
                throw std::runtime_error("Unknown Back Code.");
        }
    } else if (m_opMode == OperationMode::OP_NONE) {
        return;
    } else {
        throw std::runtime_error("Unkonwn Opretion Mode.");
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

    CShape* pShape = nullptr;

    if (m_opMode & OperationMode::OP_CREATE_RECTANGLE) {
        pShape = new CRectangle(
            CShape::z_max,
            x,
            y,
            m_filled_color,
            m_border_width,
            m_border_color,
            m_border_style
        );
    } else if (m_opMode & OperationMode::OP_CREATE_ELLIPSE) {
        pShape = new CEllipse(
            CShape::z_max,
            x,
            y,
            m_filled_color,
            m_border_width,
            m_border_color,
            m_border_style
        );
    } else if (m_opMode & OperationMode::OP_CREATE_LINE) {
        pShape = new CLine(
            CShape::z_max,
            x,
            y,
            m_filled_color,
            m_border_width,
            m_border_color,
            m_border_style
        );
    } else if (m_opMode & OperationMode::OP_CREATE_TRIANGLE) {
        pShape = new CTriangle(
            CShape::z_max,
            m_points[0].X, m_points[0].Y,
            m_points[1].X, m_points[1].Y,
            m_points[2].X, m_points[2].Y,
            m_filled_color,
            m_border_width,
            m_border_color,
            m_border_style
        );
        ClearPoints();
        pShape->setMode(EditMode::SELECT);
    } else {
        throw std::runtime_error("Unknown Create Mode.");
    }
    bool isSussussed = pDoc->AddShape(pShape);
    if (!isSussussed) {
        delete pShape; // 添加失败时释放内存
        throw std::runtime_error("Failed to add Shape.");
    }
    // 清除前一个 shape 残留的 EditMode
    ClearSelected();
    selected_shape = pShape;
    // Increase z_max
    ++CShape::z_max;
}

void CGraphicEditorView::ClearPoints() {
    m_pn = 0;
    for (int i = 0; i < MAX_POINTS; ++i)
        m_points[i].X = m_points[i].Y = -1;
}

void CGraphicEditorView::DrawPoints(Gdiplus::Graphics& graphics) {
    if (!(m_opMode & OperationMode::OP_CREATE_TRIANGLE)) return;
    // 创建画笔和画刷
    Gdiplus::Pen pen(Gdiplus::Color(255, 
                     GetRValue(m_border_color), 
                     GetGValue(m_border_color), 
                     GetBValue(m_border_color)), 
                     static_cast<Gdiplus::REAL>(m_border_width));
    // 设置线条类型
        pen.SetDashStyle(Gdiplus::DashStyleSolid);
    switch (m_border_style) {
        case PS_SOLID:
            break;
        case PS_DASH:
            pen.SetDashStyle(Gdiplus::DashStyleDash);
            break;
        case PS_DOT:
            pen.SetDashStyle(Gdiplus::DashStyleDot);
            break;
        case PS_DASHDOT:
            pen.SetDashStyle(Gdiplus::DashStyleDashDot);
            break;
        case PS_DASHDOTDOT:
            pen.SetDashStyle(Gdiplus::DashStyleDashDotDot);
            break;
        default:
            pen.SetDashStyle(Gdiplus::DashStyleSolid);
    }

    int i = 1;
    for (; i < MAX_POINTS; ++i) {
        if (m_points[i].X != -1) {
            graphics.DrawLine(&pen, 
                              static_cast<INT>(m_points[i - 1].X), 
                              static_cast<INT>(m_points[i - 1].Y), 
                              static_cast<INT>(m_points[i].X), 
                              static_cast<INT>(m_points[i].Y));
        } else break;
    }

    if (i <= 2) return;

    graphics.DrawLine(&pen, 
                      static_cast<INT>(m_points[i - 1].X), 
                      static_cast<INT>(m_points[i - 1].Y), 
                      static_cast<INT>(m_points[0].X), 
                      static_cast<INT>(m_points[0].Y));

}

void CGraphicEditorView::ClearSelected(){
    selected_shape = nullptr;
    for (int i = 0; i < selected.GetCount(); i++) {
        if (CShape* pShape = dynamic_cast<CShape*>(selected[i])) {
            pShape->setMode(EditMode::NONE);
        }
    }
    selected.RemoveAll();
    //m_opMode = OperationMode::OP_NONE;
}

bool CGraphicEditorView::AddSelected(CShape* pShape){
    if (!pShape) return false;
    if (!selected.GetCount()) {
        selected_shape = pShape;
    }
    selected.Add(pShape);
    pShape->setMode(EditMode::SELECT);
    return true;
}

bool CGraphicEditorView::IsSelected(CShape* pShape){
    if (!pShape) return false;
    for (int i = 0; i < selected.GetCount(); ++i) {
        if (selected[i] == pShape)
            return true;
    }
    return false;
}

bool CGraphicEditorView::RemoveSelected(CShape* pShape){
    if (!pShape) return false;
    for (int i = 0; i < selected.GetCount(); ++i) {
        if (selected[i] == pShape) {
            selected.RemoveAt(i);
            pShape->setMode(EditMode::NONE);
        }
    }
    //if (!selected.GetCount()) m_opMode = OperationMode::OP_NONE;
    return true;
}

bool CGraphicEditorView::CtrlClick(CPoint point){
    CGraphicEditorDoc* pDoc = GetDocument();
    if (pDoc) {
        const CObArray& shapes = pDoc->GetShapes();
        for (int i = shapes.GetCount() - 1; i >= 0; i--) {
            if (CShape* pShape = dynamic_cast<CShape*>(shapes[i])) {
                int back_code = pShape->inShape(point.x, point.y);
                if (back_code == 2) {
                    pShape = pShape->getFather();
                    if (IsSelected(pShape))
                        RemoveSelected(pShape);
                    else
                        AddSelected(pShape);
                    // ! Must break,because count has been change
                    break;
                }
            }
        }
    }
    if (!selected.GetCount()) m_opMode = OperationMode::OP_NONE;
    return true;
}

bool CGraphicEditorView::UncombSelected() {
    CGraphicEditorDoc* pDoc = GetDocument();
    const CObArray& combinations = pDoc->GetCombinations();
    const int& count = selected.GetCount();
    for (int i = count - 1; i >= 0; i--) {
        if (CCombination* pComb = dynamic_cast<CCombination*>(selected[i])) {
            RemoveSelected(pComb);
            int index = pDoc->GetIndex(pComb, combinations);
            pDoc->RemoveCombinationAt(index);
        }
    }
    m_opMode = OperationMode::OP_NONE;
    ClearSelected();
    return true;
}

bool CGraphicEditorView::CombSelected() {
    CGraphicEditorDoc* pDoc = GetDocument();

    if (!pDoc) {
        // TODO: Add error handle.
        return false;
    }

    CShape* pComb = new CCombination(selected);
    bool isSussussed = pDoc->AddCombination(pComb);
    if (!isSussussed) {
        delete pComb; // 添加失败时释放内存
        throw std::runtime_error("Failed to add Combination.");
}
    // 清除前一个 shape 残留的 EditMode
    ClearSelected();
    AddSelected(pComb);
    // Increase z_max
}

// CGraphicEditorView 消息处理程序
void CGraphicEditorView::Set_Linecolor()
{
    CColorDialog colorDlg(m_border_color);         // 构造颜色对话框，传入初始颜色值   

    if (IDOK == colorDlg.DoModal())       // 显示颜色对话框，并判断是否点击了“确定”   
    {
        m_border_color = colorDlg.GetColor();      // 获取颜色对话框中选择的颜色值   
    }
    Invalidate(); // 重绘视图
}

void CGraphicEditorView::Set_FIlledcolor()
{
    CColorDialog colorDlg(m_filled_color);         // 构造颜色对话框，传入初始颜色值   

    if (IDOK == colorDlg.DoModal())       // 显示颜色对话框，并判断是否点击了“确定”   
    {
        m_filled_color = colorDlg.GetColor();      // 获取颜色对话框中选择的颜色值   
    }
    Invalidate(); // 重绘视图
}

void CGraphicEditorView::LineMode()
{
    m_opMode = OperationMode::OP_CREATE_LINE | OperationMode::OP_SELECT | OperationMode::OP_CREATE;
}

void CGraphicEditorView::TriangleMode()
{
    ClearPoints();
    m_opMode = OperationMode::OP_CREATE_TRIANGLE | OperationMode::OP_SELECT | OperationMode::OP_CREATE;
}

void CGraphicEditorView::RectangleMode()
{
    m_opMode = OperationMode::OP_CREATE_RECTANGLE | OperationMode::OP_SELECT | OperationMode::OP_CREATE;
}

void CGraphicEditorView::EllipseMode()
{
    m_opMode = OperationMode::OP_CREATE_ELLIPSE | OperationMode::OP_SELECT | OperationMode::OP_CREATE;
}

void CGraphicEditorView::Set_LineCategory1()
{
    m_border_style = PS_SOLID;
}

void CGraphicEditorView::Set_LineCategory2()
{
    m_border_style = PS_DASH;
}


void CGraphicEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    BOOL bCtrl = GetKeyState(VK_CONTROL) < 0;
    BOOL bShift = GetKeyState(VK_SHIFT) < 0;

    switch (nChar)
    {
    case VK_ESCAPE:
        m_opMode = OperationMode::OP_NONE;
        for (int i = 0; i < m_pn; i++) {
            m_points[i] ={-1,-1};
        }
        m_pn = 0;
        if (selected_shape) selected_shape->setMode(0);
        ClearSelected();
        Invalidate();
        break;

    case 'G':
        if (bCtrl && bShift) {
            if (!selected.GetCount()) {
                return;
            }
            UncombSelected();
        } else if (bCtrl) {
            if (!selected.GetCount()) {
                return;
        }
            CombSelected();
        }
        Invalidate();
        break;
    }

    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
