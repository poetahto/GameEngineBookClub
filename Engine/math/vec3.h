#ifndef VECTOR_H
#define VECTOR_H

#include "../types.h"
#include "math_util.h"

template <u8f Length, class Type, class Derived>
struct VectorBase
{
private:
    Derived& impl()
    {
        return *static_cast<Derived*>(this);
    }

    const Derived& impl() const
    {
        return *static_cast<const Derived*>(this);
    }

public:
    const Type* data() const
    {
        Derived self = impl();
        return &self.data;
    }

    Type* data()
    {
        Derived self = impl();
        return &self.data;
    }

    Type& operator[](u8f index)
    {
        return (&impl().data)[index];
    }

    Type operator[](u8f index) const
    {
        return (&impl().data)[index];
    }

    f32 magnitude() const
    {
        return sqrt(magnitudeSqr());
    }

    f32 magnitudeSqr() const
    {
        f32 result{};

        for (u8f i = 0; i < Length; i++)
            result += (*this)[i] * (*this)[i];

        return result;
    }

    void normalize()
    {
        f32 m = magnitude();

        if (!math::nearlyEqual(m, 0))
        {
            for (u8f i = 0; i < Length; i++)
                (*this)[i] /= m;
        }
    }

    Derived normalized() const
    {
        Derived result{};

        for (u8f i = 0; i < Length; i++)
            result[i] = (*this)[i];

        result.normalize();
        return result;
    }

    static f32 dot(Derived first, Derived second)
    {
        f32 result{};

        for (u8f i = 0; i < Length; i++)
            result += first[i] * second[i];

        return result;
    }

    // todo: add lerp for floats as well?
    static Derived lerp(Derived a, Derived b, f32 t)
    {
        Derived result{};

        for (u8f i = 0; i < Length; i++)
            result[i] = (1 - t) * a[i] + t * b[i];

        return result;
    }

    bool operator !=(Derived second) const
    {
        return !(impl() == second);
    }

    template<class T>
    Derived operator *(T constant) const
    {
        Derived result{};

        for (u8f i = 0; i < Length; i++)
            result[i] = (*this)[i] * constant;

        return result;
    }

    template<class T>
    Derived operator *=(T constant)
    {
        for (u8f i = 0; i < Length; i++)
            (*this)[i] *= constant;

        return impl();
    }

    Derived operator +(Derived other) const
    {
        Derived result {};

        for (u8f i = 0; i < Length; i++)
            result[i] = (*this)[i] + other[i];

        return result;
    }

    Derived operator +=(Derived other)
    {
        for (u8f i = 0; i < Length; i++)
            (*this)[i] = (*this)[i] + other[i];

        return impl();
    }

    Derived operator -(Derived other) const
    {
        Derived result {};

        for (u8f i = 0; i < Length; i++)
            result[i] = (*this)[i] - other[i];

        return result;
    }

    Derived operator -=(Derived other)
    {
        for (u8f i = 0; i < Length; i++)
            (*this)[i] = (*this)[i] - other[i];

        return impl();
    }

    Derived operator-() const
    {
        Derived result{};

        for (u8f i = 0; i < Length; i++)
            result[i] = -(*this)[i];

        return result;
    }
};

struct vec3 : VectorBase<3, f32, vec3>
{
    union
    {
        f32 x {};
        f32 r;
        f32 data;
    };
    union
    {
        f32 y {};
        f32 g;
    };
    union
    {
        f32 z {};
        f32 b;
    };

    vec3() = default;
    vec3(f32 x, f32 y, f32 z);

    // === Operations ===
    static vec3 cross(vec3 first, vec3 second);

    // === Constants ===
    static const vec3 ZERO;
    static const vec3 ONE;
    static const vec3 UP;
    static const vec3 DOWN;
    static const vec3 LEFT;
    static const vec3 RIGHT;
    static const vec3 FORWARD;
    static const vec3 BACK;

    // === Operators ===
    bool operator==(vec3 other) const;
};


// === Operators ===
// bool operator ==(vec3 first, vec3 second); // equality
// bool operator !=(vec3 first, vec3 second); // inequality
// vec3 operator *(vec3 vector, float k); // scalar multiplication
// vec3 operator *(float k, vec3 vector);
// vec3 operator *=(vec3& first, float k);
// vec3 operator +(vec3 first, vec3 second); // addition
// vec3 operator +=(vec3& a, vec3 b);
// vec3 operator -(vec3 first, vec3 second); // subtraction
// vec3 operator -=(vec3& a, vec3 b);
// vec3 operator -(vec3 vector); // negation
std::ostream& operator<<(std::ostream& os, const vec3& value);

#endif // VECTOR_H
