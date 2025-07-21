// Rectangle.h
#pragma once

#include "Shape.h"

class CRectangle : public CShape {
public:
    CRectangle() = default;
    CRectangle(
        CDC* pDC,
        CView* pView,
        const float& z,
        const int& x,
        const int& y,
        const int& filled_color,
        const BorderWidth& border_width,
        const Color& border_color
    );

    ~CRectangle() = default;

    CCommand scale(const int& mouse_x, const int& mouse_y) override;
    CCommand rotate(float angle) override;
    bool draw() const override;

private:
    int width;
    int height;
};