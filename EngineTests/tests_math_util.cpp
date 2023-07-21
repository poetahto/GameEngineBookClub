#include <gtest/gtest.h>
#include "math/math_util.h"

using namespace math;

TEST(MathUtilTests, Inequality)
{
    EXPECT_FALSE(nearlyEqual(1, 2));
    EXPECT_FALSE(nearlyEqual(-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
    EXPECT_FALSE(nearlyEqual(-1, 1));
    EXPECT_FALSE(nearlyEqual(-0.001f, 0.001f));
    EXPECT_FALSE(nearlyEqual(0.00001f, 0.00002f));
}

TEST(MathUtilTests, EqualityToZero)
{
    EXPECT_TRUE(nearlyEqual(0, 0));
    EXPECT_TRUE(nearlyEqual(1 - 1, 0));
    EXPECT_TRUE(nearlyEqual(1.0000001f - 1.0000001f, 0));
}

TEST(MathUtilTests, EqualityReallySmall)
{
    EXPECT_TRUE(nearlyEqual(0.0125891293f, 0.0125891293f));
}

TEST(MathUtilTests, EqualityReallyBig)
{
    EXPECT_TRUE(nearlyEqual(192851872938.0f, 192851872938.0f));
}

TEST(MathUtilTests, EqualityWeird)
{
    EXPECT_TRUE(nearlyEqual(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
    EXPECT_TRUE(nearlyEqual(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()));
    EXPECT_TRUE(nearlyEqual(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
    EXPECT_TRUE(nearlyEqual(std::numeric_limits<float>::min(), std::numeric_limits<float>::min()));
}
