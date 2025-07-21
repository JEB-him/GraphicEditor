// Shape.h
#pragma once

#include "afxwin.h"
#include "../Command.h"

typedef int Color;
typedef float BorderWidth;

enum class EditMode {
    // SELECT will be used in the case when father item is selected
    // and edit it's children shape.
    SELECT  = 0b0001,  
    SCALE   = 0b0010,
    ROTATE  = 0b0100,
    DELETED = 0b1000
};

class CShape : public CObject {
public:
    CShape() = default;
    CShape(
        const CDC* pDC,
        const float& z,
        const float& x,
        const float& y,
        const float& filled_color,
        const BorderWidth& border_width,
        const Color& border_color
    );
    CShape(
        const CDC* pDC
    );
    
    virtual ~CShape() = default;

    
    // Virtual functions.
    virtual bool update();
    /**
     * @brief move the shape or the combination
     * @note  the combination neeed a unique implemention.
     */
    virtual CCommand move(const int& x, const int& y);
    /**
     * @brief scale the shape or the combination
     * @note  the combination neeed a unique implemention.
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
    virtual void draw(CDC* pDC) const = 0;
    /**
     * @brief Rotate the graphic.
     * @note  Note the boundary of the window.
     */
    virtual CCommand rotate(float angle) = 0;

protected:
    // The initial width and height of a shape
    const static float LENGTH;
    // A CDC pointer to draw shape.
    const CDC* pDC;

    // the max z pos
    static float z_max;
    // z pos
    // The range is [0, 100)
    // In any edit mode, the z will lose efficacy and
    // the controller must draw the item at the end.
    float z;

    // Original smallest rectangle
    // Position, taken from the smallest x and smallest y.
    // (0, 0) ---- (+, 0)
    // |                |
    // |                |
    // (0, +) ---- (+, +)
    float old_x;
    float old_y;
    float old_bwidth;
    float old_bheight;

    // New smallest rectangle
    float new_x;
    float new_y;
    float new_bwidth;
    float new_bheight;

    // padding
    Color filled_color;

    // Border
    BorderWidth border_width;
    Color border_color;

    // Mode
    int mode;
};