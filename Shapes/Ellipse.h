
// Ellipse.h
#pragma once

#include "Shape.h"

class CEllipse : public CShape {
public:
    CEllipse() = default;
    CEllipse(
        const float& z,
        const int& x,
        const int& y,
        const Color& filled_color,
        const BorderWidth& border_width,
        const Color& border_color,
        const BorderStyle border_style
    );

    ~CEllipse() = default;

    CCommand scale(CView* pView, const int& mouse_x, const int& mouse_y) override;
    CCommand move(CView* pView, const int& x, const int& y) override;
    CCommand rotate(float angle) override;
    int inShape(const int&x, const int&y) const override;
    bool draw(Gdiplus::Graphics& graphics) override;

private:
    int x;
    int y;
    int width;
    int height;
};
