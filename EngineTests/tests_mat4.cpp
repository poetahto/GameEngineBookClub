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
