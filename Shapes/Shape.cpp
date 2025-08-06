#include "pch.h"
#include "Shape.h"

#include <cmath>
#include <stdexcept>
#include <iostream>

const int CShape::LENGTH = 1;
float CShape::z_max = 0;

CShape::CShape(
    const float& z,
    const int& x,
    const int& y,
    const Color& filled_color,
    const BorderWidth& border_width,
    const Color& border_color,
    const BorderStyle border_style) :
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
    ) {
}

bool CShape::setMode(EditMode mode) {
    this->mode = mode;
    return true;
}

const float& CShape::getZ() const
{
    return this->z;
}

// 序列化方法
void CShape::Serialize(CArchive& ar) {
    CObject::Serialize(ar);
    if (ar.IsStoring()) {
        // 存储数据
        ar << LENGTH;
        ar << z;
        ar << old_x;
        ar << old_y;
        ar << old_bwidth;
        ar << old_bheight;
        ar << new_x;
        ar << new_y;
        ar << new_bwidth;
        ar << new_bheight;
        ar << filled_color;
        ar << border_style;
        ar << border_width;
        ar << border_color;
        ar << mode;
    } else {
        // 加载数据
        ar >> z;
        ar >> old_x;
        ar >> old_y;
        ar >> old_bwidth;
        ar >> old_bheight;
        ar >> new_x;
        ar >> new_y;
        ar >> new_bwidth;
        ar >> new_bheight;
        ar >> filled_color;
        ar >> border_style;
        ar >> border_width;
        ar >> border_color;
        mode = EditMode::NONE;
    }
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
CCommand CShape::move(CView* pView, const int& x, const int& y) {
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

CCommand CShape::scale(CView* pView, const int& mouse_x, const int& mouse_y) {
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
