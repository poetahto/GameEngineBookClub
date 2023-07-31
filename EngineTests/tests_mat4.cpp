﻿#include <gtest/gtest.h>
#include "math/mat4.h"
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
    mat4 e = c * d;

    mat4 ex = {
        {{0, 114, 0, 0}, {0, 0, 0, 0}, {-118, 0, -708, 0}, {0, 0, 0, 0}}
    };
    EXPECT_EQ(e, ex);
}
