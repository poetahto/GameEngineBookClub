#ifndef VECTOR_H
#define VECTOR_H

#include "../types.h"
#include "math_util.h"

/**
 * \brief A templated base for vectors, allowing types and sizes to be swapped
 * without duplicate code.
 * \tparam Length The length of the vector (vec3 = 3, vec2 = 2, ect.)
 * \tparam Type The data stored by the vector (vec3 = f32, vec3int = i32, ect.)
 * \tparam Derived The class that is deriving this one (used for CRTP).
 */
template <u8f Length, class Type, class Derived>
struct VectorBase
{
    /**
     * \brief Determines the magnitude (or length) of a vector.
     * \return The magnitude of the vector.
     * \note If possible, use magnitudeSqr() instead for better performance.
     */
    f32 magnitude() const
    {
        return sqrt(magnitudeSqr());
    }

    /**
     * \brief Determines the magnitude (or length) of a vector, squared.
     * \return The magnitude of the vector to the power of 2.
     */
    f32 magnitudeSqr() const
    {
        f32 result{};

        for (u8f i = 0; i < Length; i++)
            result += (*this)[i] * (*this)[i];

        return result;
    }

    /**
     * \brief Normalizes this vector (ensures a magnitude of 1).
     */
    void normalize()
    {
        f32 m = magnitude();

        if (!Math::nearlyEqual(m, 0))
        {
            for (u8f i = 0; i < Length; i++)
                (*this)[i] /= m;
        }
    }

    /**
     * \brief Computes a normalized version of this vector, without actually changing it.
     * \return A normalized version of this vector.
     */
    Derived normalized() const
    {
        Derived result{};

        for (u8f i = 0; i < Length; i++)
            result[i] = (*this)[i];

        result.normalize();
        return result;
    }

    /**
     * \brief Computes the dot product between two vectors.
     * \param first The first vector.
     * \param second The second vector.
     * \return The dot product between the two vectors.
     */
    static f32 dot(Derived first, Derived second)
    {
        f32 result{};

        for (u8f i = 0; i < Length; i++)
            result += first[i] * second[i];

        return result;
    }

    // todo: add lerp for floats as well?
    /**
     * \brief Linearly interpolates between two vectors and a percent.
     * \param a The first vector.
     * \param b The second vector.
     * \param t The percent blend between the two vectors.
     * \return An interpolated vector between a and b, by t.
     */
    static Derived lerp(Derived a, Derived b, f32 t)
    {
        Derived result{};

        for (u8f i = 0; i < Length; i++)
            result[i] = (1 - t) * a[i] + t * b[i];

        return result;
    }

    // === Operators ===

    // Subscript
    Type& operator[](u8f index)
    {
        return (&impl().data)[index];
    }

    Type operator[](u8f index) const
    {
        return (&impl().data)[index];
    }

    // Equality
    bool operator !=(Derived second) const
    {
        return !(impl() == second);
    }

    // Multiplication
    template<class T>
    Derived operator *(T constant) const
    {
        Derived result{};

        for (u8f i = 0; i < Length; i++)
            result[i] = (*this)[i] * static_cast<f32>(constant);

        return result;
    }

    template<class T>
    Derived operator *=(T constant)
    {
        for (u8f i = 0; i < Length; i++)
            (*this)[i] *= static_cast<f32>(constant);

        return impl();
    }

    // Addition
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

    // Subtraction
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

    // Negation
    Derived operator-() const
    {
        Derived result{};

        for (u8f i = 0; i < Length; i++)
            result[i] = -(*this)[i];

        return result;
    }
private:
    // === CRTP Helpers ===
    Derived& impl()
    {
        return *static_cast<Derived*>(this);
    }

    const Derived& impl() const
    {
        return *static_cast<const Derived*>(this);
    }
};

#endif // VECTOR_H
