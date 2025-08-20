#include "pch.h"
#include "Combination.h"

#include <stdexcept>

CCombination::CCombination(const CObArray& arr) : CShape() {
    children.Copy(arr);
    int left = 0x7fffffff, top = 0x7fffffff, bottom = 0, right = 0;
    for (int i = children.GetCount() - 1; i >= 0; i--) {
        if (CShape* pShape = dynamic_cast<CShape*>(children[i])) {
            pShape->setFather(this);
            CRect rect = pShape->getRect();
            left   = min(left, rect.left);
            top    = min(top, rect.top);
            right  = max(right, rect.right);
            bottom = max(bottom, rect.bottom);
        }
    }
    new_x = left, new_y = top;
    new_bwidth = right - left, new_bheight = bottom - top;
}

CCombination::~CCombination(){
    for (int i = children.GetCount() - 1; i >= 0; i--) {
        if (CShape* pShape = dynamic_cast<CShape*>(children[i])) {
            pShape->setFather(pShape);
        }
    }
}

void CCombination::updateRect() {
    int left = 0x7fffffff, top = 0x7fffffff, bottom = 0, right = 0;
    for (int i = children.GetCount() - 1; i >= 0; i--) {
        if (CShape* pShape = dynamic_cast<CShape*>(children[i])) {
            CRect rect = pShape->getRect();
            left = min(left, rect.left);
            top = min(top, rect.top);
            right = max(right, rect.right);
            bottom = max(bottom, rect.bottom);
        }
    }
    new_x = left, new_y = top;
    new_bwidth = right - left, new_bheight = bottom - top;
}

int CCombination::setMode(const unsigned int& mode)
{
    this->mode = mode;
    unsigned int child_mode = mode;
    if (mode & EditMode::SELECT) child_mode ^= SELECT;
    for (int i = children.GetCount() - 1; i >= 0; i--) {
        if (CShape* pShape = dynamic_cast<CShape*>(children[i])) {
            pShape->setMode(child_mode);
        }
    }
    return 0;
}

int CCombination::addMode(const unsigned int& mode)
{
    this->mode |= mode;
    unsigned int child_mode = mode;
    if (mode & EditMode::SELECT) child_mode ^= SELECT;
    for (int i = children.GetCount() - 1; i >= 0; i--) {
        if (CShape* pShape = dynamic_cast<CShape*>(children[i])) {
            pShape->addMode(child_mode);
        }
    }
    return 0;
}

CCommand CCombination::scale(CView* pView, const int& mouse_x, const int& mouse_y) {
    int tmp_width = new_bwidth;
    CCommand cmd = CShape::scale(pView, mouse_x, mouse_y);
    float mult = new_bwidth / tmp_width;
    // TODO: Validate the cmd.
    for (int i = 0; i < children.GetCount(); i++) {
        if (CShape* pChild = dynamic_cast<CShape*>(children[i])) {
            CRect rect = pChild->getRect();
            int x = rect.left + rect.Width() * mult;
            int y = rect.top + rect.Height() * mult;
            pChild->scale(pView, x, y);
        }
    }
    updateRect();
    return cmd;
}

CCommand CCombination::fill(const Color& filled_color){
    for (int i = 0; i < children.GetCount(); i++) {
        if (CShape* pChild = dynamic_cast<CShape*>(children[i])) {
            pChild->fill(filled_color);
        }
    }
    return {};
}

CCommand CCombination::move(CView* pView, const int& x, const int& y) {
    int tmp_x = new_x, tmp_y = new_y;
    CCommand cmd = CShape::move(pView, x, y);
    int horizon = new_x - tmp_x;
    int vertical = new_y - tmp_y;
    // TODO: Validate the cmd.
    for (int i = 0; i < children.GetCount(); i++) {
        if (CShape* pChild = dynamic_cast<CShape*>(children[i])) {
            CRect rect = pChild->getRect();
            int des_x = rect.left + horizon;
            int des_y = rect.top + vertical;
            pChild->move(pView, des_x, des_y);
        }
    }
    return cmd;
}

bool CCombination::draw(Gdiplus::Graphics& graphics) {
    drawSelectedBorder(graphics);
    return true;
}

CCommand CCombination::rotate(float angle) {
    return {};
}

CCommand CCombination::modifyBorder(const Color& border_color, const int& border_width){
    for (int i = 0; i < children.GetCount(); i++) {
        if (CShape* pChild = dynamic_cast<CShape*>(children[i])) {
            pChild->modifyBorder(border_color, border_width);
        }
    }
    return {};
}

int CCombination::inShape(const int& x, const int& y) const {
    // 1. 检查是否在虚线框右下角范围内
    if (x >= (new_x + new_bwidth - CShape::SCOPE) && x <= (new_x + new_bwidth + CShape::SCOPE) &&
        y >= (new_y + new_bheight - CShape::SCOPE) && y <= (new_y + new_bheight + CShape::SCOPE)) {
        return 0;
    }
    if (x >= new_x - CShape::DIFF && x <= new_x + new_bwidth + CShape::DIFF &&
        y >= new_y - CShape::DIFF && y <= new_y + new_bheight + CShape::DIFF) {
        return 1;
    }
    return -1;
}
