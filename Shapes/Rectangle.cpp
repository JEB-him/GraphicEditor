#include "pch.h"
#include "Rectangle.h"

#include <stdexcept>

CRectangle::CRectangle(
    const float& z,
    const int& x,
    const int& y,
    const Color& filled_color,
    const BorderWidth& border_width,
    const Color& border_color,
    const BorderStyle border_style) :
    CShape(
        z, x, y,
        filled_color,
        border_width,
        border_color,
        border_style
    ),
    x(),
    y(),
    width(),
    height(){
}

CCommand CRectangle::scale(CView* pView, const int& mouse_x, const int& mouse_y) {
    CCommand cmd = CShape::scale(pView, mouse_x, mouse_y);
    // TODO: Validate the cmd.
    width  = new_bwidth - CShape::DIFF;
    height = new_bheight - CShape::DIFF;
    if (width < 0 || height < 0) {
        width  = new_bwidth * 2 / 3;
        height = new_bheight * 2 / 3;
        x = new_x + new_bwidth / 6;
        y = new_y + new_bheight / 6;
    } else {
        x = new_x + 1;
        y = new_y + 1;
    }
    return cmd;
}

bool CRectangle::draw(Gdiplus::Graphics& graphics) {
    drawSelectedBorder(graphics);
    // 创建画笔和画刷
    Gdiplus::Pen pen(Gdiplus::Color(255, 
                     GetRValue(border_color), 
                     GetGValue(border_color), 
                     GetBValue(border_color)), 
                     static_cast<Gdiplus::REAL>(border_width));
    // 设置线条类型
    switch (border_style) {
    case PS_SOLID:
        pen.SetDashStyle(Gdiplus::DashStyleSolid);
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

    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 
                     GetRValue(filled_color), 
                     GetGValue(filled_color), 
                     GetBValue(filled_color)));

    // 绘制填充矩形
    graphics.FillRectangle(&brush, 
                           static_cast<INT>(x), 
                           static_cast<INT>(y), 
                           static_cast<INT>(width), 
                           static_cast<INT>(height));

    // 绘制边框
    graphics.DrawRectangle(&pen, 
                           static_cast<INT>(x), 
                           static_cast<INT>(y), 
                           static_cast<INT>(width), 
                           static_cast<INT>(height));
    return true;
}

CCommand CRectangle::rotate(float angle) {
    return {};
}

