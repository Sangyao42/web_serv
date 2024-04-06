#pragma once

#include <gtest/gtest.h>

#include "Configuration/Directive/Block/Main.hpp"
#include "Configuration/Directive/Block/Http.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Block/Events.hpp"
#include "Configuration/Directive/Block/Location.hpp"

/**
 * [] MainBlock
 * [] HttpBlock
 * [] ServerBlock
 * [] EventsBlock
 * [] LocationBlock
*/

class TestDirectiveMain : public ::testing::Test
{
  protected:
    directive::MainBlock test_target_;
};

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
