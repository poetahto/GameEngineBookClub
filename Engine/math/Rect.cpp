﻿#include "Rect.h"

f32 Rect::width() const
{
    return right - left;
}

f32 Rect::height() const
{
    return top - bottom;
}

