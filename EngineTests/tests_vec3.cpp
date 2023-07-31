#include <gtest/gtest.h>
#include "math/vec3.h"

using namespace testing;

TEST(Vec3Tests, Equality)
{
    EXPECT_EQ(vec3::ZERO, vec3::ZERO);
    EXPECT_EQ(vec3(0.124123f, 4, 19248123.0f), vec3(0.124123f, 4, 19248123.0f));
    EXPECT_NE(vec3(9812481924.0f, 0.0000000000123f, 1.0f), vec3(0.124123f, 4, 19248123.0f));
    EXPECT_NE(vec3::ZERO, vec3::ONE);
}

TEST(Vec3Tests, Normalize)
{
    EXPECT_EQ(vec3::ZERO.normalized(), vec3::ZERO);
    EXPECT_EQ(vec3(25, 0, 0).normalized(), vec3(1, 0, 0));
    EXPECT_EQ(vec3(-25, 0, 0).normalized(), vec3(-1, 0, 0));
    EXPECT_EQ(vec3(0, 10, 0).normalized(), vec3(0, 1, 0));
    EXPECT_EQ(vec3(0, -10, 0).normalized(), vec3(0, -1, 0));
    EXPECT_EQ(vec3(0, 0, 10).normalized(), vec3(0, 0, 1));
    EXPECT_EQ(vec3(0, 0, -10).normalized(), vec3(0, 0, -1));
}

TEST(Vec3Tests, Magnitude)
{
    EXPECT_EQ(vec3(5, 0, 0).magnitude(), 5);
    EXPECT_EQ(vec3(0, 5, 0).magnitude(), 5);
    EXPECT_EQ(vec3(0, 0, 5).magnitude(), 5);
    EXPECT_EQ(vec3(-5, 0, 0).magnitude(), 5);
    EXPECT_EQ(vec3(0, -5, 0).magnitude(), 5);
    EXPECT_EQ(vec3(0, 0, -5).magnitude(), 5);
    EXPECT_EQ(vec3::ZERO.magnitude(), 0);

    EXPECT_EQ(vec3::ZERO.magnitudeSqr(), 0);
    EXPECT_EQ(vec3(0, 5, 0).magnitudeSqr(), 25);
}

TEST(Vec3Tests, DotProduct)
{
    EXPECT_EQ(vec3::dot(vec3(1, 0, 0), vec3(0, 1, 0)), 0);
    EXPECT_EQ(vec3::dot(vec3(5, 0, 0), vec3(5, 0, 0)), 25);
    EXPECT_EQ(vec3::dot(vec3(-5, 0, 0), vec3(5, 0, 0)), -25);
}

TEST(Vec3Tests, CrossProduct)
{
    EXPECT_EQ(vec3::cross(vec3::ZERO, vec3::ZERO), vec3::ZERO);
    EXPECT_EQ(vec3::cross(vec3(1, 5, -2), vec3(8, -242, 12)), vec3(-424, -28, -282));
    EXPECT_EQ(vec3::cross(vec3(1, 5, -2), vec3::ZERO), vec3::ZERO);
}

TEST(Vec3Tests, Lerp)
{
    vec3 a { 10, 0, 0 };
    vec3 b { 10, 20, 30 };
    EXPECT_EQ(vec3::lerp(a, b, 0), a);
    EXPECT_EQ(vec3::lerp(a, b, 1), b);
    EXPECT_EQ(vec3::lerp(a, b, 0.5f), vec3(10, 10, 15));
}

TEST(Vec3Tests, Subtraction)
{
    EXPECT_EQ(vec3::ONE - vec3::ONE, vec3::ZERO);
    EXPECT_EQ(vec3::ZERO - vec3::ZERO, vec3::ZERO);
    EXPECT_EQ(vec3::ONE - vec3::ZERO, vec3::ONE);
    EXPECT_EQ(vec3::ZERO - vec3::ONE, -vec3::ONE);

    vec3 r = vec3 { 94, 28, -12 };
    r -= vec3{24, 20, -10};
    EXPECT_EQ(r, vec3(70, 8, -2));
}

TEST(Vec3Tests, Negation)
{
    EXPECT_EQ(-vec3::ZERO, vec3::ZERO);
    EXPECT_EQ(-vec3::ONE, vec3(-1, -1, -1));
}

TEST(Vec3Tests, Addition)
{
    EXPECT_EQ(vec3::ONE + vec3::ONE, vec3(2, 2, 2));
    EXPECT_EQ(vec3::ZERO + vec3::ZERO, vec3::ZERO);

    vec3 r = vec3 { 5, 1, -4 };
    r += vec3 { -2, -1, 2};

    EXPECT_EQ(r, vec3(3, 0, -2));
}

TEST(Vec3Tests, ScalarMultiplication)
{
    {
        vec3 v{1, 1, 1};
        v *= 5;

        EXPECT_EQ(v.x, 5);
        EXPECT_EQ(v.y, 5);
        EXPECT_EQ(v.z, 5);
    }
    {
        vec3 v{1, 1, 1};
        v = v * 5;

        EXPECT_EQ(v.x, 5);
        EXPECT_EQ(v.y, 5);
        EXPECT_EQ(v.z, 5);
    }
    {
        vec3 v{1, 1, 1};
        v = 5 * v;

        EXPECT_EQ(v.x, 5);
        EXPECT_EQ(v.y, 5);
        EXPECT_EQ(v.z, 5);
    }
}
