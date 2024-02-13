#pragma once

#include <gtest/gtest.h>

class ExampleP : public ::testing::TestWithParam<int>
{
protected:
  int test_target_;
  void SetUp() override
  {
    test_target_ = 1;
    // This is called before each test
  }

  void TearDown() override
  {
    test_target_ = 0;
    // This is called after each test
  }
};
