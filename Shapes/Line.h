
// Line.h
#pragma once

#include "Shape.h"

class CLine : public CShape {
public:
    CLine() = default;
    CLine(
        const float& z,
        const int& x,
        const int& y,
        const Color& filled_color,
        const BorderWidth& border_width,
        const Color& border_color,
        const BorderStyle border_style
    );

    ~CLine() = default;

    CCommand scale(CView* pView, const int& mouse_x, const int& mouse_y) override;
    CCommand rotate(float angle) override;
    bool inShape(const int&x, const int&y) override;
    bool draw(Gdiplus::Graphics& graphics) override;

private:
    int sx;
    int sy;
    int ex;
    int ey;
};
