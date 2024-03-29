#pragma once

#include <gtest/gtest.h>

#include "Configuration/Directive/Blocks.hpp"

/**
 * [] HttpBlock
 * [] ServerBlock
 * [] EventsBlock
 * [] LocationBlock
*/

class TestDirectiveHttp : public ::testing::Test
{
  protected:
    directive::HttpBlock test_target_;
};

class TestDirectiveServer : public ::testing::Test
{
  protected:
    directive::ServerBlock test_target_;
};

class TestDirectiveEvents : public ::testing::Test
{
  protected:
    directive::EventsBlock test_target_;
};

class TestDirectiveLocation : public ::testing::Test
{
  protected:
    directive::LocationBlock  test_target_;
};
