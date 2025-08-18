
// Line.cpp
#include "pch.h"
#include "Line.h"

#include <stdexcept>

CLine::CLine(
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
    sx(),
    sy(),
    ex(),
    ey(){
}

CCommand CLine::scale(CView* pView, const int& mouse_x, const int& mouse_y) {
    CCommand cmd =  {};
    CRect clientRect;
    pView->GetClientRect(&clientRect);

    int width  = clientRect.Width();
    int height = clientRect.Height();

    if (mouse_x < 0 || mouse_y < 0 || mouse_x > clientRect.Width() || mouse_y > clientRect.Height()) {
        throw std::logic_error("x/y is invalid.");
    }
    // TODO: mirror scaling function need be added.
    // int mult = std::abs(mouse_x - new_x) / new_bwidth >
    //              std::abs(mouse_y - new_y) / new_bheight ? 
    //              std::abs(mouse_x - new_x) / new_bwidth :
    //              std::abs(mouse_y - new_y) / new_bheight;

    if (mouse_x <= new_x || mouse_y <= new_y) {
        return {};
    }

    new_bwidth  = mouse_x - new_x;
    new_bheight = mouse_y - new_y;

    // TODO: Validate the cmd.
    if (new_bwidth < CShape::DIFF || new_bheight < CShape::DIFF) {
        sx = new_x + new_bwidth / 6;
        sy = new_y + new_bheight / 6;
        ex = sx + new_bwidth * 2 / 3;
        ey = sy + new_bheight * 2 / 3;
    } else {
        sx = new_x + 1;
        sy = new_y + 1;
        ex = sx + new_bwidth - CShape::DIFF;
        ey = sy + new_bheight - CShape::DIFF;
    }
    return cmd;
}

CCommand CLine::move(CView* pView, const int& x, const int& y) override {
    // 调用基类 move()
    CShape::move(pView, x, y);
    this->sx = new_x + CShape::DIFF;
    this->sy = new_y + CShape::DIFF;
    this->ex = new_x - CShape::DIFF + new_bwidth;
    this->ey = new_y - CShape::DIFF + new_bheight;
}

bool CLine::draw(Gdiplus::Graphics& graphics) {
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

    // 绘制边框
    graphics.DrawLine(&pen, 
                           static_cast<INT>(sx), 
                           static_cast<INT>(sy), 
                           static_cast<INT>(ex), 
                           static_cast<INT>(ey));
    return true;
}

CCommand CLine::rotate(float angle) {
    return {};
}

bool CLine::inShape(const int&x, const int&y) const {
    // TODO By Hao Jiang: Compeleted it.
    return false;
}
