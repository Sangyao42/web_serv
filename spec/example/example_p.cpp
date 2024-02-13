#include "example_p.hpp"

#include <gtest/gtest.h>

TEST_P(ExampleP, example_for_testp)
{
  int param = GetParam();
  ASSERT_EQ(param * test_target_, param);
}

INSTANTIATE_TEST_SUITE_P(ExampleP, ExampleP, testing::Values(
	1,
  3,
  5
));
