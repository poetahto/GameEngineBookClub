#include <gtest/gtest.h>
#include "math/mat4.h"
#include "math/math_util.h"
#include "math/vector_base.h"

using namespace math;

TEST(Mat4Tests, Equality)
{
    EXPECT_EQ(Mat4::ZERO, Mat4::ZERO);
    EXPECT_FALSE(Mat4::ZERO == Mat4::IDENTITY);
}

TEST(Mat4Tests, Inequality)
{
    EXPECT_NE(Mat4::IDENTITY, Mat4::ZERO);
    EXPECT_FALSE(Mat4::IDENTITY != Mat4::IDENTITY);
}

TEST(Mat4Tests, ArrayIndex)
{
    Mat4 mat = Mat4::ZERO;
    mat[3][2] = 1;
    EXPECT_EQ(mat[3][2], 1);
    mat[3][2] = 0;
    EXPECT_EQ(mat[3][2], 0);
}

TEST(Mat4Tests, Transpose)
{
    EXPECT_EQ(Mat4::IDENTITY, Mat4::IDENTITY.transpose());

    Mat4 m
    {
        {
            {1, 1, 1, 1},
            {0, 2, 0, 0},
            {0, 0, 3, 0},
            {0, 2, 0, 4},
        }
    };

    Mat4 expected
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
        Mat4 m = Mat4::translate(5, 0, 1);
        EXPECT_EQ(m.getTranslation(), Vec3(5, 0, 1));
    }
    {
        Mat4 m = Mat4::translate(5, 0, 1) * Mat4::scale(2);
        EXPECT_EQ(m.getTranslation(), Vec3(10, 0, 2));
    }
    {
        Mat4 m = Mat4::translate(5, 0, 0) * Mat4::rotateY(90 * DEG2RAD);
        EXPECT_EQ(m.getTranslation(), Vec3(0, 0, -5));
    }
}

TEST(MatrixTest, Inverse)
{
    {
        Mat4 m = Mat4::translate(5, 0, 1);
        EXPECT_EQ(m.inverse() * m, Mat4::IDENTITY);
    }
    {
        Mat4 m = Mat4::rotateY(55 * DEG2RAD);
        EXPECT_EQ(m.inverse() * m, Mat4::IDENTITY);
    }
    {
        Mat4 m = Mat4::scale(581);
        EXPECT_EQ(m.inverse() * m, Mat4::IDENTITY);
    }
    {
        Mat4 m = Mat4::scale(581) * Mat4::rotateX(23 * DEG2RAD) * Mat4::translate(48, 12, -15);
        EXPECT_EQ(m.inverse() * m, Mat4::IDENTITY);
    }
    {
        Mat4 m = Mat4::rotateY(35 * DEG2RAD) * Mat4::rotateX(-25 * DEG2RAD) * Mat4::translate(-42, -42, 19) * Mat4::scale(5, 2, -1);
        EXPECT_EQ(m.inverse() * m, Mat4::IDENTITY);
    }
}

TEST(MatrixTest, Basis)
{
    Mat4 m{Mat4::IDENTITY};
    EXPECT_EQ(m.up(), Vec3::UP);
    EXPECT_EQ(m.right(), Vec3::RIGHT);
    EXPECT_EQ(m.forward(), Vec3::FORWARD);

    m = m * Mat4::rotateY(90 * DEG2RAD) * Mat4::translate(4, 29, 123) * Mat4::scale(2);
    EXPECT_EQ(m.up(), Vec3::UP);
    EXPECT_EQ(m.right(), -Vec3::FORWARD);
    EXPECT_EQ(m.forward(), Vec3::RIGHT);
}

TEST(MatrixTest, RotationZ)
{
    Vec3 v{0, 1, 0};
    Vec3 expected{-1, 0, 0};
    EXPECT_EQ(Mat4::rotateZ(90 * DEG2RAD).transformPoint(v), expected);
}

TEST(MatrixTest, RotationY)
{
    Vec3 v{1, 0, 0};
    Vec3 expected{0, 0, -1};
    EXPECT_EQ(Mat4::rotateY(90 * DEG2RAD).transformPoint(v), expected);
}

TEST(MatrixTest, RotationX)
{
    Vec3 v{0, 1, 0};
    Vec3 expected{0, 0, 1};
    EXPECT_EQ(Mat4::rotateX(90 * DEG2RAD).transformPoint(v), expected);
}

TEST(MatrixText, Translation)
{
    Vec3 v{0, 0, 0};
    Vec3 expected{1, 0, 0};
    EXPECT_EQ(Mat4::translate(1, 0, 0).transformPoint(v), expected);
}

TEST(MatrixTest, Scale)
{
    Vec3 v{1, 1, 1};
    Vec3 expected{2, 2, 2};
    EXPECT_EQ(Mat4::scale(2).transformPoint(v), expected);
}

TEST(MatrixTest, Equality)
{
    Mat4 a{Mat4::IDENTITY};
    Mat4 b{Mat4::IDENTITY};
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
    Mat4 a{Mat4::IDENTITY};
    Mat4 b{};
    b[3][1] = 5;
    b[2][3] = -59;
    b[0][2] = 19;

    Mat4 c = a * b;

    Mat4 expected = {
        {{0, 0, 19, 0}, {0, 0, 0, 0}, {0, 0, 0, -59}, {0, 5, 0, 0}}
    };
    EXPECT_EQ(c, expected);

    Mat4 d = {{{3, 0, 4, 124}, {0, 0, 0, 0}, {0, 6, 0, 0}, {2, 0, 12, 0}}};
    Mat4 e {Mat4::IDENTITY};
    e *= c * d;

    Mat4 ex = {
        {{0, 114, 0, 0}, {0, 0, 0, 0}, {-118, 0, -708, 0}, {0, 0, 0, 0}}
    };
    EXPECT_EQ(e, ex);
}
