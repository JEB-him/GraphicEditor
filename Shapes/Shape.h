// Shape.h
#pragma once

#include "afxwin.h"
#include "Command.h"

typedef COLORREF Color;
typedef int      BorderWidth;
typedef int      BorderStyle;

constexpr Color BRUSH_TRANSPARENT = RGB(255, 255, 255);

enum EditMode : unsigned int { // SELECT will be used in the case when father item is selected
    // and edit it's children shape.
    NONE    = 0b00000,
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

    //CShape() = default;
    CShape(
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
    );

    // TODO copy constructor
    const float& getZ() const;
    const unsigned int& getMode() const;
    int addMode(const unsigned int& mode);
    // TODO: Should overrided by combination.
    virtual int setMode(const unsigned int& mode);

    virtual ~CShape() = default;

    // 序列化方法
    virtual void Serialize(CArchive& ar) override;

    
    // Virtual functions.
    virtual bool update();
    // Must react to window-resize event.
    virtual bool resize();
    /**
     * @brief move the shape or the combination
     * @note  The combination needs a unique implemention.
     */
    virtual CCommand move(CView* pView, const int& x, const int& y);
    /**
     * @brief scale the shape or the combination
     * @note  Every children class needs a unique implemention.
     */
    virtual CCommand scale(CView* pView, const int& mouse_x, const int& mouse_y);
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
    // TODO: 当增删成员变量的时候修改对应序列化代码

    // The initial width and height of a shape
    const static int LENGTH;
    // A pointer point to view.

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
    unsigned int mode = EditMode::CREATE;
};
