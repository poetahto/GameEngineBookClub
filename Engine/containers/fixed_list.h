#ifndef FIXED_LIST_H
#define FIXED_LIST_H

#include <vector>

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

    s32 sum() const
    {
        s32 result {};

        for (s32 i = 0; i < count; i++)
            result += data[i];

        return result;
    }

    static FixedList fromList(std::vector<T> list)
    {
        s32 size = list.size();
        T* data = new T[size]; // todo: custom allocator support?

        for (s32 i = 0; i < size; i++)
            data[i] = list[i];

        return FixedList{size,  data};
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
