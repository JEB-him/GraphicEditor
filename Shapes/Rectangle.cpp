#include "pch.h"
#include "Rectangle.h"

CRectangle::CRectangle(
    CDC* pDC,
    const float& z,
    const float& x,
    const float& y,
    const float& filled_color,
    const BorderWidth& border_width,
    const Color& border_color) :
    CShape(
        pDC,
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
    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(pDC, width, height);
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

    memDC.FillSolidRect(new_x, new_y, width, height, RGB(0, 0, 255));  // 绘制矩形

    pDC->BitBlt(new_y, new_y, width, height, &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldBitmap);
}