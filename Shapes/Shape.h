// Shape.h
#pragma once

#include "afxwin.h"
#include "Command.h"

typedef COLORREF Color;
typedef int      BorderWidth;
typedef int      BorderStyle;

constexpr Color BRUSH_TRANSPARENT = RGB(255, 255, 255);

enum EditMode : unsigned int {
    // SELECT will be used in the case when father item is selected
    // and edit it's children shape.
    SELECT  = 0b00001,
    SCALE   = 0b00010,
    ROTATE  = 0b00100,
    DELETED = 0b01000,
    CREATE  = 0b10000
};

class CShape : public CObject {
public:
    // the max z pos
    static float z_max;

    CShape() = default;
    CShape(
        CView* PView,
        const float& z,
        const int& x,
        const int& y,
        const Color& filled_color,
        const BorderWidth& border_width,
        const Color& border_color,
        const BorderStyle border_style
    );

    // The combination needn't set layer.
    CShape(
        CView* PView
    );

    bool setMode(EditMode mode);

    virtual ~CShape() = default;

    
    // Virtual functions.
    virtual bool update();
    // Must react to window-resize event.
    virtual bool resize();
    /**
     * @brief move the shape or the combination
     * @note  The combination needs a unique implemention.
     */
    virtual CCommand move(const int& x, const int& y);
    /**
     * @brief scale the shape or the combination
     * @note  Every children class needs a unique implemention.
     */
    virtual CCommand scale(const int& mouse_x, const int& mouse_y);
    /**
     * @brief Modify the filled color of the graphic.
     * @note  The line need a unique implemention.
     */
    virtual CCommand fill(const Color& filled_color);

    /**
     * @brief Modify the border's properties.
     * @note  Maybe it will affect the size of smallest rectangle!
     */
    virtual CCommand modifyBorder(const Color& border_color, const int& border_width);

    // Pure virtual functions.
    virtual bool draw(Gdiplus::Graphics & graphics) = 0;
    /**
     * @brief Rotate the graphic.
     * @note  Note the boundary of the window.
     */
    virtual CCommand rotate(float angle) = 0;

protected:
    // The initial width and height of a shape
    const static int LENGTH;
    // A pointer point to view.
    CView*  pView;

    // z pos
    // The range is [0, 100)
    // In any edit mode, the z will lose efficacy and
    // the controller must draw the item at the end.
    float z = -1;

    // Original smallest rectangle
    // Position, taken from the smallest x and smallest y.
    // (0, 0) ---- (+, 0)
    // |                |
    // |                |
    // (0, +) ---- (+, +)
    int old_x       = -1;
    int old_y       = -1;
    int old_bwidth  = -1;
    int old_bheight = -1;

    // New smallest rectangle
    int new_x       = -1;
    int new_y       = -1;
    int new_bwidth  = -1;
    int new_bheight = -1;

    // padding
    Color filled_color {};

    // Border
    BorderStyle border_style {};
    BorderWidth border_width {};
    Color border_color {};

    // Mode
    EditMode mode = EditMode::CREATE;
};
