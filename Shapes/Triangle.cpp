// Triangle.cpp
#include "pch.h"
#include "Triangle.h"

#include <stdexcept>

CTriangle::CTriangle(
    const float& z,
    const int&x1, const int&y1,
    const int&x2, const int&y2,
    const int&x3, const int&y3,
    const Color& filled_color,
    const BorderWidth& border_width,
    const Color& border_color,
    const BorderStyle border_style) :
    CShape(
        z, -1, -1,
        filled_color,
        border_width,
        border_color,
        border_style
    ),
    points {PointF(x1, y1), PointF(x2, y2), PointF(x3, y3)} {
        CRect rect = getSmallRect();
        if (rect.left > CShape::DIFF) rect.left = rect.left - CShape::DIFF;
        if (rect.top > CShape::DIFF) rect.top = rect.top - CShape::DIFF;
        this->new_x = rect.left, this->new_y = rect.top;
}

CRect CTriangle::getSmallRect() {
    int left_x = (points[1].X < points[2].X ? points[1].X : points[2].X);
    left_x = (left_x < points[3].X ? left_x : points[3].X);
    int right_x = (points[1].X > points[2].X ? points[1].X : points[2].X);
    right_x = (right_x > points[3].X ? right_x : points[3].X);
  
    int down_y = (points[1].Y < points[2].Y ? points[1].Y : points[2].Y);
    down_y = (down_y < points[3].Y ? down_y : points[3].Y);
    int up_y = (points[1].Y > points[2].Y ? points[1].Y : points[2].Y);
    up_y = (up_y > points[3].Y ? up_y : points[3].Y);
  
    return {left_x, up_y, right_x, down_y};
}

CCommand CTriangle::scale(CView* pView, const int& mouse_x, const int& mouse_y) {
    CRect clientRect;
    pView->GetClientRect(&clientRect);

    int width  = clientRect.Width();
    int height = clientRect.Height();

    if (mouse_x < 0 || mouse_y < 0 || mouse_x > clientRect.Width() || mouse_y > clientRect.Height()) {
        throw std::logic_error("x/y is invalid.");
    }

    if (mouse_x <= new_x || mouse_y <= new_y) {
        return {};
    }

    int mult = mouse_x - new_x / new_bwidth >
                 mouse_y - new_y / new_bheight ? 
                 mouse_x - new_x / new_bwidth :
                 mouse_y - new_y / new_bheight;

    new_bwidth  *= mult;
    new_bheight *= mult;

    for (int i = 0;i < 3; ++i) {
        points[i].X = (points[i].X - new_x - CShape::DIFF) * mult + new_x + CShape::DIFF;
        points[i].Y = (points[i].Y - new_x - CShape::DIFF) * mult + new_x + CShape::DIFF;
    }

    return {};
}

bool CTriangle::draw(Gdiplus::Graphics& graphics) {
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

    // 绘制填充三角形
    graphics.FillPolygon(&brush, points, 3);

    // 绘制边框
    graphics.DrawPolygon(&pen, points, 3);
    return true;
}

CCommand CTriangle::rotate(float angle) {
    return {};
}

