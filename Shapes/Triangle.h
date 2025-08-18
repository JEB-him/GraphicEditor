// Triangle.h
#pragma once

#include "Shape.h"

class CTriangle : public CShape {
public:
    CTriangle() = default;
    CTriangle(
        const float& z,
        const int&x1, const int&y1,
        const int&x2, const int&y2,
        const int&x3, const int&y3,
        const Color& filled_color,
        const BorderWidth& border_width,
        const Color& border_color,
const BorderStyle border_style
    );

    ~CTriangle() = default;

    CRect getSmallRect() const;

    CCommand scale(CView* pView, const int& mouse_x, const int& mouse_y) override;
    CCommand move(CView* pView, const int& x, const int& y) override;
    CCommand rotate(float angle) override;
    bool inShape(const int&x, const int&y) const override;
    bool draw(Gdiplus::Graphics& graphics) override;

private:
    Gdiplus::PointF points[3];  // 三角形的三个顶点
};
