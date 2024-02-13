#include "example_f.hpp"

#include <gtest/gtest.h>

TEST_F(ExampleF, example_for_testf)
{
  ASSERT_EQ(test_target_, 1);
}
