#include <gtest/gtest.h>
#include "../Engine/math/vec3.hpp"

using namespace testing;

TEST(Vec3Tests, Equality)
{
    EXPECT_EQ(Vec3::ZERO, Vec3::ZERO);
    EXPECT_EQ(Vec3(0.124123f, 4, 19248123.0f), Vec3(0.124123f, 4, 19248123.0f));
    EXPECT_NE(Vec3(9812481924.0f, 0.0000000000123f, 1.0f), Vec3(0.124123f, 4, 19248123.0f));
    EXPECT_NE(Vec3::ZERO, Vec3::ONE);
}

TEST(Vec3Tests, Normalize)
{
    EXPECT_EQ(Vec3::ZERO.normalized(), Vec3::ZERO);
    EXPECT_EQ(Vec3(25, 0, 0).normalized(), Vec3(1, 0, 0));
    EXPECT_EQ(Vec3(-25, 0, 0).normalized(), Vec3(-1, 0, 0));
    EXPECT_EQ(Vec3(0, 10, 0).normalized(), Vec3(0, 1, 0));
    EXPECT_EQ(Vec3(0, -10, 0).normalized(), Vec3(0, -1, 0));
    EXPECT_EQ(Vec3(0, 0, 10).normalized(), Vec3(0, 0, 1));
    EXPECT_EQ(Vec3(0, 0, -10).normalized(), Vec3(0, 0, -1));
}

TEST(Vec3Tests, Magnitude)
{
    EXPECT_EQ(Vec3(5, 0, 0).magnitude(), 5);
    EXPECT_EQ(Vec3(0, 5, 0).magnitude(), 5);
    EXPECT_EQ(Vec3(0, 0, 5).magnitude(), 5);
    EXPECT_EQ(Vec3(-5, 0, 0).magnitude(), 5);
    EXPECT_EQ(Vec3(0, -5, 0).magnitude(), 5);
    EXPECT_EQ(Vec3(0, 0, -5).magnitude(), 5);
    EXPECT_EQ(Vec3::ZERO.magnitude(), 0);

    EXPECT_EQ(Vec3::ZERO.magnitudeSqr(), 0);
    EXPECT_EQ(Vec3(0, 5, 0).magnitudeSqr(), 25);
}

TEST(Vec3Tests, DotProduct)
{
    EXPECT_EQ(Vec3::dot(Vec3(1, 0, 0), Vec3(0, 1, 0)), 0);
    EXPECT_EQ(Vec3::dot(Vec3(5, 0, 0), Vec3(5, 0, 0)), 25);
    EXPECT_EQ(Vec3::dot(Vec3(-5, 0, 0), Vec3(5, 0, 0)), -25);
}

TEST(Vec3Tests, CrossProduct)
{
    EXPECT_EQ(Vec3::cross(Vec3::ZERO, Vec3::ZERO), Vec3::ZERO);
    EXPECT_EQ(Vec3::cross(Vec3(1, 5, -2), Vec3(8, -242, 12)), Vec3(-424, -28, -282));
    EXPECT_EQ(Vec3::cross(Vec3(1, 5, -2), Vec3::ZERO), Vec3::ZERO);
}

TEST(Vec3Tests, Lerp)
{
    Vec3 a { 10, 0, 0 };
    Vec3 b { 10, 20, 30 };
    EXPECT_EQ(Vec3::lerp(a, b, 0), a);
    EXPECT_EQ(Vec3::lerp(a, b, 1), b);
    EXPECT_EQ(Vec3::lerp(a, b, 0.5f), Vec3(10, 10, 15));
}

TEST(Vec3Tests, Subtraction)
{
    EXPECT_EQ(Vec3::ONE - Vec3::ONE, Vec3::ZERO);
    EXPECT_EQ(Vec3::ZERO - Vec3::ZERO, Vec3::ZERO);
    EXPECT_EQ(Vec3::ONE - Vec3::ZERO, Vec3::ONE);
    EXPECT_EQ(Vec3::ZERO - Vec3::ONE, -Vec3::ONE);

    Vec3 r = Vec3 { 94, 28, -12 };
    r -= Vec3{24, 20, -10};
    EXPECT_EQ(r, Vec3(70, 8, -2));
}

TEST(Vec3Tests, Negation)
{
    EXPECT_EQ(-Vec3::ZERO, Vec3::ZERO);
    EXPECT_EQ(-Vec3::ONE, Vec3(-1, -1, -1));
}

TEST(Vec3Tests, Addition)
{
    EXPECT_EQ(Vec3::ONE + Vec3::ONE, Vec3(2, 2, 2));
    EXPECT_EQ(Vec3::ZERO + Vec3::ZERO, Vec3::ZERO);

    Vec3 r = Vec3 { 5, 1, -4 };
    r += Vec3 { -2, -1, 2};

    EXPECT_EQ(r, Vec3(3, 0, -2));
}

TEST(Vec3Tests, ScalarMultiplication)
{
    {
        Vec3 v{1, 1, 1};
        v *= 5;

        EXPECT_EQ(v.x, 5);
        EXPECT_EQ(v.y, 5);
        EXPECT_EQ(v.z, 5);
    }
    {
        Vec3 v{1, 1, 1};
        v = v * 5;

        EXPECT_EQ(v.x, 5);
        EXPECT_EQ(v.y, 5);
        EXPECT_EQ(v.z, 5);
    }
}
