#include "pch.h"
#include "Rectangle.h"

CRectangle::CRectangle(
    CDC* pDC,
    CView* pView,
    const float& z,
    const int& x,
    const int& y,
    const int& filled_color,
    const BorderWidth& border_width,
    const Color& border_color) :
    CShape(
        pDC,
        pView,
        z, x, y,
        filled_color,
        border_width,
        border_color
    ),
    width(LENGTH),
    height(LENGTH){
}

CCommand CRectangle::scale(const int& mouse_x, const int& mouse_y) {
    CCommand cmd = CShape::scale(mouse_x, mouse_y);
    // TODO: Validate the cmd.
    width  = new_bwidth;
    height = new_bheight;
    return cmd;
}

bool CRectangle::draw() const {
    // TODO: draw border, shape.
    // Use solid brush as default.
    // 获取客户区大小
    CRect clientRect;
    pView->GetClientRect(&clientRect);  // 获取窗口客户区的尺寸

    // 创建内存设备上下文
    CDC memDC;
    memDC.CreateCompatibleDC(pDC);

    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(pDC, new_bwidth, new_bheight);
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

    // 绘制矩形
    memDC.FillSolidRect(0, 0, width, height, filled_color);  // 蓝色矩形

    // 将内存中的图像复制到屏幕设备上下文
    pDC->BitBlt(new_x, new_y, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);

    // 恢复内存设备上下文的位图
    memDC.SelectObject(pOldBitmap);
    return true;
}

CCommand CRectangle::rotate(float angle) {
    return {};
}