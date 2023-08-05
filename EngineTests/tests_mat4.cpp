#include <gtest/gtest.h>
#include "math/mat4.h"
#include "..\Engine\math\vector_base.h"
#include "math/math_util.h"

using namespace math;

TEST(Mat4Tests, Equality)
{
    EXPECT_EQ(mat4::ZERO, mat4::ZERO);
    EXPECT_FALSE(mat4::ZERO == mat4::IDENTITY);
}

TEST(Mat4Tests, Inequality)
{
    EXPECT_NE(mat4::IDENTITY, mat4::ZERO);
    EXPECT_FALSE(mat4::IDENTITY != mat4::IDENTITY);
}

TEST(Mat4Tests, ArrayIndex)
{
    mat4 mat = mat4::ZERO;
    mat[3][2] = 1;
    EXPECT_EQ(mat[3][2], 1);
    mat[3][2] = 0;
    EXPECT_EQ(mat[3][2], 0);
}

TEST(Mat4Tests, Transpose)
{
    EXPECT_EQ(mat4::IDENTITY, mat4::IDENTITY.transpose());

    mat4 m
    {
        {
            {1, 1, 1, 1},
            {0, 2, 0, 0},
            {0, 0, 3, 0},
            {0, 2, 0, 4},
        }
    };

    mat4 expected
    {
        {
            {1, 0, 0, 0},
            {1, 2, 0, 2},
            {1, 0, 3, 0},
            {1, 0, 0, 4},
        }
    };

    EXPECT_EQ(m.transpose(), expected);
}

TEST(MatrixTest, GetPosition)
{
    {
        mat4 m = mat4::translate(5, 0, 1);
        EXPECT_EQ(m.getTranslation(), vec3(5, 0, 1));
    }
    {
        mat4 m = mat4::translate(5, 0, 1) * mat4::scale(2);
        EXPECT_EQ(m.getTranslation(), vec3(10, 0, 2));
    }
    {
        mat4 m = mat4::translate(5, 0, 0) * mat4::rotateY(90 * DEG2RAD);
        EXPECT_EQ(m.getTranslation(), vec3(0, 0, -5));
    }
}

TEST(MatrixTest, Inverse)
{
    {
        mat4 m = mat4::translate(5, 0, 1);
        EXPECT_EQ(m.inverse() * m, mat4::IDENTITY);
    }
    {
        mat4 m = mat4::rotateY(55 * DEG2RAD);
        EXPECT_EQ(m.inverse() * m, mat4::IDENTITY);
    }
    {
        mat4 m = mat4::scale(581);
        EXPECT_EQ(m.inverse() * m, mat4::IDENTITY);
    }
    {
        mat4 m = mat4::scale(581) * mat4::rotateX(23 * DEG2RAD) * mat4::translate(48, 12, -15);
        EXPECT_EQ(m.inverse() * m, mat4::IDENTITY);
    }
    {
        mat4 m = mat4::rotateY(35 * DEG2RAD) * mat4::rotateX(-25 * DEG2RAD) * mat4::translate(-42, -42, 19) * mat4::scale(5, 2, -1);
        EXPECT_EQ(m.inverse() * m, mat4::IDENTITY);
    }
}

TEST(MatrixTest, Basis)
{
    mat4 m{mat4::IDENTITY};
    EXPECT_EQ(m.up(), vec3::UP);
    EXPECT_EQ(m.right(), vec3::RIGHT);
    EXPECT_EQ(m.forward(), vec3::FORWARD);

    m = m * mat4::rotateY(90 * DEG2RAD) * mat4::translate(4, 29, 123) * mat4::scale(2);
    EXPECT_EQ(m.up(), vec3::UP);
    EXPECT_EQ(m.right(), -vec3::FORWARD);
    EXPECT_EQ(m.forward(), vec3::RIGHT);
}

TEST(MatrixTest, RotationZ)
{
    vec3 v{0, 1, 0};
    vec3 expected{-1, 0, 0};
    EXPECT_EQ(mat4::rotateZ(90 * DEG2RAD).transformPoint(v), expected);
}

TEST(MatrixTest, RotationY)
{
    vec3 v{1, 0, 0};
    vec3 expected{0, 0, -1};
    EXPECT_EQ(mat4::rotateY(90 * DEG2RAD).transformPoint(v), expected);
}

TEST(MatrixTest, RotationX)
{
    vec3 v{0, 1, 0};
    vec3 expected{0, 0, 1};
    EXPECT_EQ(mat4::rotateX(90 * DEG2RAD).transformPoint(v), expected);
}

TEST(MatrixText, Translation)
{
    vec3 v{0, 0, 0};
    vec3 expected{1, 0, 0};
    EXPECT_EQ(mat4::translate(1, 0, 0).transformPoint(v), expected);
}

TEST(MatrixTest, Scale)
{
    vec3 v{1, 1, 1};
    vec3 expected{2, 2, 2};
    EXPECT_EQ(mat4::scale(2).transformPoint(v), expected);
}

TEST(MatrixTest, Equality)
{
    mat4 a{mat4::IDENTITY};
    mat4 b{mat4::IDENTITY};
    EXPECT_EQ(a, b);

    a[0][0] = 3;
    b[0][0] = 3;
    EXPECT_EQ(a, b);

    a[3][2] = -5;
    b[3][2] = -5;
    EXPECT_EQ(a, b);
}

TEST(MatrixTest, Multiplication)
{
    mat4 a{mat4::IDENTITY};
    mat4 b{};
    b[3][1] = 5;
    b[2][3] = -59;
    b[0][2] = 19;

    mat4 c = a * b;

    mat4 expected = {
        {{0, 0, 19, 0}, {0, 0, 0, 0}, {0, 0, 0, -59}, {0, 5, 0, 0}}
    };
    EXPECT_EQ(c, expected);

    mat4 d = {{{3, 0, 4, 124}, {0, 0, 0, 0}, {0, 6, 0, 0}, {2, 0, 12, 0}}};
    mat4 e {mat4::IDENTITY};
    e *= c * d;

    mat4 ex = {
        {{0, 114, 0, 0}, {0, 0, 0, 0}, {-118, 0, -708, 0}, {0, 0, 0, 0}}
    };
    EXPECT_EQ(e, ex);
}
