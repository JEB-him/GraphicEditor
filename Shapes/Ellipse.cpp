
// Ellipse.cpp
#include "pch.h"
#include "Ellipse.h"

#include <stdexcept>

CEllipse::CEllipse(
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

CCommand CEllipse::scale(CView* pView, const int& mouse_x, const int& mouse_y) {
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
        x = new_x + CShape::DIFF/2;
        y = new_y + CShape::DIFF/2;
    }
    return cmd;
}

CCommand CEllipse::move(CView* pView, const int& x, const int& y){
    // 调用基类 move()
    CShape::move(pView, x, y);
    this->x = new_x + CShape::DIFF/2;
    this->y = new_y + CShape::DIFF/2;
    return { };
}

bool CEllipse::draw(Gdiplus::Graphics& graphics) {
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
    graphics.FillEllipse(&brush, 
                           static_cast<INT>(x), 
                           static_cast<INT>(y), 
                           static_cast<INT>(width), 
                           static_cast<INT>(height));

    // 绘制边框
    graphics.DrawEllipse(&pen, 
                           static_cast<INT>(x), 
                           static_cast<INT>(y), 
                           static_cast<INT>(width), 
                           static_cast<INT>(height));
    return true;
}

CCommand CEllipse::rotate(float angle) {
    return {};
}

int CEllipse::inShape(const int&x, const int&y) const {
    if (x >= (new_x + new_bwidth - CShape::SCOPE) &&
        y >= (new_y + new_bheight - CShape::SCOPE)) {
        return 0;
    }

    // 2. 检查是否在虚线框及附近范围内
    if ((x >= (new_x - CShape::SCOPE) && x <= (new_x + new_bwidth + CShape::SCOPE)) &&
        (y >= (new_y - CShape::SCOPE) && y <= (new_y + new_bheight + CShape::SCOPE))) {
        return 1;
    }

    // 3. 检查是否在椭圆内部或边界上
    // 椭圆方程: ((x - centerX)^2)/a^2 + ((y - centerY)^2)/b^2 <= 1
    float centerX = this->x + width / 2.0f;
    float centerY = this->y + height / 2.0f;
    float a = width / 2.0f;
    float b = height / 2.0f;

    float normalizedX = (x - centerX) / a;
    float normalizedY = (y - centerY) / b;

    if ((normalizedX * normalizedX + normalizedY * normalizedY) <= 1.0f) {
        return 2;
    }

    // 4. 都不满足
    return -1;

    return false;
}
