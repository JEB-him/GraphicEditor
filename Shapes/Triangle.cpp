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
    points {Gdiplus::PointF(x1, y1), Gdiplus::PointF(x2, y2), Gdiplus::PointF(x3, y3)} {
        CRect rect = getSmallRect();
        if (rect.left > CShape::DIFF) rect.left = rect.left - CShape::DIFF / 2;
        if (rect.top > CShape::DIFF) rect.top = rect.top - CShape::DIFF / 2;
        if (rect.right > CShape::DIFF) rect.right = rect.right + CShape::DIFF / 2;
        if (rect.bottom > CShape::DIFF) rect.bottom = rect.bottom + CShape::DIFF / 2;
        this->new_x = rect.left, this->new_y = rect.top;
        this->new_bheight = rect.bottom - rect.top;
        this->new_bwidth = rect.right - rect.left;
}

CRect CTriangle::getSmallRect() const {
    int left_x = (points[0].X < points[1].X ? points[0].X : points[1].X);
    left_x = (left_x < points[2].X ? left_x : points[2].X);
    int right_x = (points[0].X > points[1].X ? points[0].X : points[1].X);
    right_x = (right_x > points[2].X ? right_x : points[2].X);
  
    int up_y = (points[0].Y < points[1].Y ? points[0].Y : points[1].Y);
    up_y = (up_y < points[2].Y ? up_y : points[2].Y);
    int down_y = (points[0].Y > points[1].Y ? points[0].Y : points[1].Y);
    down_y = (down_y > points[2].Y ? down_y : points[2].Y);
  
    return {left_x, up_y, right_x, down_y};
}

bool CTriangle::updateRect()
{
    CRect rect = getSmallRect();
    if (rect.left > CShape::DIFF) rect.left = rect.left - CShape::DIFF / 2;
    if (rect.top > CShape::DIFF) rect.top = rect.top - CShape::DIFF / 2;
    if (rect.right > CShape::DIFF) rect.right = rect.right + CShape::DIFF / 2;
    if (rect.bottom > CShape::DIFF) rect.bottom = rect.bottom + CShape::DIFF / 2;
    this->new_x = rect.left, this->new_y = rect.top;
    this->new_bheight = rect.bottom - rect.top;
    this->new_bwidth = rect.right - rect.left;
    return true;
}

CCommand CTriangle::scale(CView* pView, const int& mouse_x, const int& mouse_y) {
    CRect clientRect;
    pView->GetClientRect(&clientRect);

    int width  = clientRect.Width();
    int height = clientRect.Height();

    if (mouse_x < 0 || mouse_y < 0 || mouse_x > clientRect.Width() || mouse_y > clientRect.Height()) {
        throw std::logic_error("x/y is invalid.");
    }

    if (mouse_x <= new_x + CShape::DIFF || mouse_y <= new_y + CShape::DIFF) {
        return {};
    }

    float mult;
    float multx = (float)(mouse_x - new_x) / new_bwidth;
    float multy = (float)(mouse_y - new_y) / new_bheight;
    if (multx > multy) mult = multx;
    else mult = multy;
    new_bwidth *= mult;
    new_bheight *= mult;

    for (int i = 0;i < 3; ++i) {
        points[i].X = (points[i].X - new_x) * mult + new_x;
        points[i].Y = (points[i].Y - new_y) * mult + new_y;
    }

    return {};
}

CCommand CTriangle::move(CView* pView, const int& x, const int& y) {
    // 先求相对坐标
    float rposx[3], rposy[3];
    for (int i = 0; i < 3; ++i) {
        rposx[i] = points[i].X - new_x;
        rposy[i] = points[i].Y - new_y;
    }
    // 调用基类 move()
    CShape::move(pView, x, y);
    for (int i = 0; i < 3; ++i) {
        points[i].X = rposx[i] + new_x;
        points[i].Y = rposy[i] + new_y;
    }
    return { };
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

int CTriangle::inShape(const int&x, const int&y) const {
    // 1. 检查是否在虚线框右下角范围内
    if (x >= (new_x + new_bwidth - CShape::SCOPE) && x <= (new_x + new_bwidth + CShape::SCOPE) &&
        y >= (new_y + new_bheight - CShape::SCOPE) && y <= (new_y + new_bheight + CShape::SCOPE)) {
        return 0;
    }

    // 3. 检查是否在三角形内部
    // 使用重心坐标法判断点是否在三角形内
    auto sign = [](Gdiplus::PointF p1, Gdiplus::PointF p2, Gdiplus::PointF p3) {
        return (p1.X - p3.X) * (p2.Y - p3.Y) - (p2.X - p3.X) * (p1.Y - p3.Y);
    };

    Gdiplus::PointF pt(static_cast<float>(x), static_cast<float>(y));
    float d1 = sign(pt, points[0], points[1]);
    float d2 = sign(pt, points[1], points[2]);
    float d3 = sign(pt, points[2], points[0]);

    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    if (!(hasNeg && hasPos)) {
        return 2;
    }
    // 2. 检查是否在虚线框及附近范围内
    if (x >= (new_x - CShape::SCOPE) && x <= (new_x + new_bwidth + CShape::SCOPE) && (y >= (new_y - CShape::SCOPE) && y <= (new_y + new_bheight + CShape::SCOPE)))
        return 1;
    // 4. 都不满足
    return -1;
}
