// Combination.h
#pragma conce

#include "Shape.h"

class CCombination : public CShape {
public:
    CCombination(const CObArray& arr= CObArray());
    ~CCombination() = default;

    int setMode(const unsigned int& mode) override;
    int addMode(const unsigned int& mode) override;

    CCommand scale(CView* pView, const int& mouse_x, const int& mouse_y) override;
    CCommand fill(const Color& filled_color) override;
    CCommand move(CView* pView, const int& x, const int& y) override;
    CCommand rotate(float angle) override;
    CCommand modifyBorder(const Color& border_color, const int& border_width) override;
    int inShape(const int&x, const int&y) const override;
    bool draw(Gdiplus::Graphics& graphics) override;

private:
    CObArray children;
};
