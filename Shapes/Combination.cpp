#include "pch.h"
#include "Combination.h"

#include <stdexcept>

CCombination::CCombination(const CobArray& arr) : CShape(), children(arr) {}

CCommand CCombination::scale(CView* pView, const int& mouse_x, const int& mouse_y) {
    int tmp_width = new_bwidth;
    CCommand cmd = CShape::scale(pView, mouse_x, mouse_y);
    int mult = new_bwidth / tmp_width;
    // TODO: Validate the cmd.
    for (int i = 0; i < children.GetCount(); i++) {
        if (CShape* pChild = dynamic_cast<CShape*>(children[i])) {
            CRect rect = pChild->getRect();
            int x = rect.right + rect.width * mult;
            int y = rect.bottom + rect.height * mult;
            pChild->scale(pView, x, y);
        }
    }
    return cmd;
}

CCommand CCombination::move(CView* pView, const int& x, const int& y) override {
    int tmp_x = new_x, tmp_y = new_y;
    CCommand cmd = CShape::move(pView, x, y);
    int horizon = new_x - tmp_x;
    int vertical = new_y - tmp_y;
    // TODO: Validate the cmd.
    for (int i = 0; i < children.GetCount(); i++) {
        if (CShape* pChild = dynamic_cast<CShape*>(children[i])) {
            CRect rect = pChild->getRect();
            int des_x = rect.right + horizon;
            int des_y = rect.bottom + vertical;
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

int CCombination::inShape(const int& x, const int& y) const {
    // TODO By Hao Jiang: Complete it!
    return -1;
}
