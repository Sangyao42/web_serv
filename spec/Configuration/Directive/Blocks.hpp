#pragma once

#include <gtest/gtest.h>

#include "Configuration/Directive/Blocks.hpp"

/**
 * [] DirectiveHttp
 * [] DirectiveServer
 * [] DirectiveEvents
 * [] DirectiveLocation
*/

class TestDirectiveHttp : public ::testing::Test
{
  protected:
    configuration::DirectiveHttp test_target_;
};

class TestDirectiveServer : public ::testing::Test
{
  protected:
    configuration::DirectiveServer test_target_;
};

class TestDirectiveEvents : public ::testing::Test
{
  protected:
    configuration::DirectiveEvents test_target_;
};

class TestDirectiveLocation : public ::testing::Test
{
  protected:
    configuration::DirectiveLocation  test_target_;
};
