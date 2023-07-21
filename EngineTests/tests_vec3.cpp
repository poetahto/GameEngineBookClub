#include <gtest/gtest.h>
#include "math/vec3.h"

using namespace testing;

TEST(Vec3Tests, Equality)
{
    EXPECT_EQ(ZERO, ZERO);
    EXPECT_EQ(vec3(0.124123f, 4, 19248123.0f), vec3(0.124123f, 4, 19248123.0f));
    EXPECT_NE(vec3(9812481924.0f, 0.0000000000123f, 1.0f), vec3(0.124123f, 4, 19248123.0f));
    EXPECT_NE(ZERO, ONE);
}

TEST(Vec3Tests, Magnitude)
{
    EXPECT_EQ(vec3(5, 0, 0).magnitude(), 5);
    EXPECT_EQ(ZERO.magnitude(), 0);

    EXPECT_EQ(ZERO.magnitudeSqr(), 0);
    EXPECT_EQ(vec3(0, 5, 0).magnitudeSqr(), 25);
}

TEST(Vec3Tests, Subtraction)
{
    EXPECT_EQ(ONE - ONE, ZERO);
    EXPECT_EQ(ZERO - ZERO, ZERO);
    EXPECT_EQ(ONE - ZERO, ONE);
    EXPECT_EQ(ZERO - ONE, -ONE);
}

TEST(Vec3Tests, Negation)
{
    EXPECT_EQ(-ZERO, ZERO);
    EXPECT_EQ(-ONE, vec3(-1, -1, -1));
}

TEST(Vec3Tests, Addition)
{
    EXPECT_EQ(ONE + ONE, vec3(2, 2, 2));
    EXPECT_EQ(ZERO + ZERO, ZERO);
}

TEST(Vec3Tests, ScalarMultiplication)
{
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
