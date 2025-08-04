#include "pch.h"
#include "Shape.h"

#include <cmath>
#include <stdexcept>
#include <iostream>

const int CShape::LENGTH = 1;
float CShape::z_max = 0;

CShape::CShape(
    CView* pView,
    const float& z,
    const int& x,
    const int& y,
    const Color& filled_color,
    const BorderWidth& border_width,
    const Color& border_color,
    const BorderStyle border_style) :
    pView(pView),
    z(z),
    new_x(x),
    new_y(y),
    new_bwidth(LENGTH),
    new_bheight(LENGTH),
    filled_color(filled_color),
    border_width(border_width),
    border_color(border_color),
    border_style(border_style) {
}

CShape::CShape(
    CView* pView) :
    pView(pView) {
}

bool CShape::setMode(EditMode mode) {
    this->mode = mode;
    return true;
}

bool CShape::update() {
    old_x       = new_x;
    old_y       = new_y;
    old_bwidth  = new_bwidth;
    old_bheight = new_bheight;
    return true;
}

bool CShape::resize() {
    return false;
}

/**
 * @param x
 * @param y
 * @return
 */
CCommand CShape::move(const int& x, const int& y) {
    CRect clientRect;
    pView->GetClientRect(&clientRect);

    int width  = clientRect.Width();
    int height = clientRect.Height();

    if (x < 0 || y < 0 || x > width || y > height) {
        throw std::logic_error("x/y is invalid.");
    }

    if (!(width - x < new_bwidth)) {
        new_x = x;
    }

    if (!(height - y < new_bheight)) {
        new_y = y;
    }

    // TODO: implement return statement.
    return {};
}

CCommand CShape::scale(const int& mouse_x, const int& mouse_y) {
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

    // TODO: If the width/length is set to 0 after the left mouse
    // button is released, the controller should react to this r-
    // eset the size of the shape.
    if (mouse_x < new_x || mouse_y < new_y) {
        return {};
    }

    if (mode & EditMode::CREATE) {
        new_bwidth  = mouse_x - new_x;
        new_bheight = mouse_y - new_y;
        return {};
    }

    int mult = mouse_x - new_x / new_bwidth >
                 mouse_y - new_y / new_bheight ? 
                 mouse_x - new_x / new_bwidth :
                 mouse_y - new_y / new_bheight;

    new_bwidth  *= mult;
    new_bheight *= mult;

    return {};
}

CCommand CShape::fill(const Color& filled_color) {
    // TODO: Check the validation of the color
    if (false) {

    }
    this->filled_color = filled_color;
    return {};
}

CCommand CShape::modifyBorder(const Color& border_color, const int& border_width) {
    // TODO: Check the validation of the color
    if (false) {

    }
    this->border_color = border_color;
    this->border_width = border_width;

    return {};
}
