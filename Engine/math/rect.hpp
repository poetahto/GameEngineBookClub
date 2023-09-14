#ifndef RECT_H
#define RECT_H

#include "../types.hpp"

struct Rect
{
    f32 left;
    f32 right;
    f32 top;
    f32 bottom;

    f32 width() const;
    f32 height() const;
};

#endif // RECT_H
