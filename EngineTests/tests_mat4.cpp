#include <gtest/gtest.h>
#include "math/mat4.h"

TEST(Mat4Tests, Equality)
{
    EXPECT_EQ(ZERO, ZERO);
    EXPECT_FALSE(ZERO == IDENTITY);
}

TEST(Mat4Tests, Inequality)
{
    EXPECT_NE(IDENTITY, ZERO);
    EXPECT_FALSE(IDENTITY != IDENTITY);
}

TEST(Mat4Tests, ArrayIndex)
{
    mat4 mat = ZERO;
    mat[3][2] = 1;
    EXPECT_EQ(mat[3][2], 1);
    mat[3][2] = 0;
    EXPECT_EQ(mat[3][2], 0);
}

// TEST(MatrixTest, XRotation)
// {
//     mat4 r{mat4::rotation(Vector3{180 * DEG2RAD, 0, 0})};
//     mat4 e{mat4::identity()};
//     e[0][0] = 1;
//     e[1][1] = -1;
//     e[2][2] = -1;
//     EXPECT_EQ(r, e);
// }
//
// TEST(MatrixTest, YRotation)
// {
//     mat4 r{mat4::rotation(Vector3{0, 180 * DEG2RAD, 0})};
//     mat4 e{mat4::identity()};
//     e[0][0] = -1;
//     e[1][1] = 1;
//     e[2][2] = -1;
//     EXPECT_EQ(r, e);
// }
//
// TEST(MatrixTest, ZRotation)
// {
//     mat4 r{mat4::rotation(Vector3{0, 0, 180 * DEG2RAD})};
//     mat4 e{mat4::identity()};
//     e[0][0] = -1;
//     e[1][1] = -1;
//     e[2][2] = 1;
//     EXPECT_EQ(r, e);
// }

// TEST(MatrixTest, FullRotation)
// {
//     mat4 a{mat4::translation(Vector3{1, 0, 0})};
//     mat4 b{mat4::rotation(Vector3{0, 0, 360 * DEG2RAD})};
//     mat4 r{b * a};
//     mat4 e{a};
//     EXPECT_EQ(r, e) << "B: " << b << "\nA: " << a;
// }
//
// TEST(MatrixTest, ZeroRotation)
// {
//     mat4 a{mat4 {}};
//     mat4 b{mat4::rotation(Vector3{0, 0, 180 * DEG2RAD})};
//     mat4 r{b * a};
//     mat4 e{mat4 {}};
//     EXPECT_EQ(r, e) << "B: " << b << "\nA: " << a;
// }

TEST(MatrixTest, Equality)
{
    mat4 a { IDENTITY };
    mat4 b { IDENTITY };
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
    mat4 a { IDENTITY };
    mat4 b {};
    b[3][1] = 5;
    b[2][3] = -59;
    b[0][2] = 19;

    mat4 c = a * b;

    mat4 expected = {
        {{0, 0, 19, 0}, {0, 0, 0, 0}, {0, 0, 0, -59}, {0, 5, 0, 0}}};
    EXPECT_EQ(c, expected);

    mat4 d = {{{3, 0, 4, 124}, {0, 0, 0, 0}, {0, 6, 0, 0}, {2, 0, 12, 0}}};
    mat4 e = c * d;

    mat4 ex = {
        {{0, 114, 0, 0}, {0, 0, 0, 0}, {-118, 0, -708, 0}, {0, 0, 0, 0}}};
    EXPECT_EQ(e, ex);
}
