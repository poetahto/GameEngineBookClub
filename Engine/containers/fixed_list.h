#ifndef FIXED_LIST_H
#define FIXED_LIST_H

#include "../types.h"

template <class T>
struct FixedList
{
    s32 count;
    T* data;

    // === Functions ===
    s32 getSize() const
    {
        return count * sizeof(T);
    }

    // === Operators ===
    T& operator[](u32 index)
    {
        return data[index];
    }

    T operator[](u32 index) const
    {
        return data[index];
    }
};

#endif // FIXED_LIST_H
