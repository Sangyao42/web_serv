#pragma once

#include <gtest/gtest.h>

#include "Configuration/Directive/Simple.hpp"

/**
 * [/] DirectiveListen
 * [/] DirectiveServerName
 * [/] DirectiveAllowMethods
 * [ ] DirectiveRoot
 * [ ] DirectiveIndex
 * [/] DirectiveMimeTypes
 * [ ] DirectiveErrorPage
 * [ ] DirectiveClientMaxBodySize
 * [/] DirectiveRedirect
 * [ ] DirectiveAutoindex
 * [/] DirectiveCgi
 * [ ] DirectiveAccessLog
 * [ ] DirectiveErrorLog
 * [ ] DirectiveInclude
 * [ ] DirectiveWorkerConnections
*/

class TestDirectiveListen :
  public ::testing::TestWithParam<std::vector<configuration::DirectiveListen::IpAddress> >
{
protected:
  configuration::DirectiveListen test_target_;
  void SetUp() override {}
  void TearDown() override {}
};

class TestDirectiveServerName :
  public ::testing::TestWithParam<std::vector<std::string> >
{
protected:
  configuration::DirectiveServerName test_target_;
  void SetUp() override {}
  void TearDown() override {}
};

class TestDirectiveAllowMethods :
  public ::testing::TestWithParam<int>
{
protected:
  configuration::DirectiveAllowMethods test_target_;
  void SetUp() override {}
  void TearDown() override {}
};

using MimeTypeVector = std::vector<std::pair<std::string, std::string> >;

class TestDirectiveMimeTypes :
  public ::testing::TestWithParam<MimeTypeVector>
{
protected:
  configuration::DirectiveMimeTypes test_target_;
  void SetUp() override {}
  void TearDown() override {}
};

class TestFilledDirectiveMimeTypes : public ::testing::Test
{
protected:
  configuration::DirectiveMimeTypes test_target_;
  void SetUp() override
  {
    test_target_.add("html", "text/html");
    test_target_.add("htm", "text/html");
    test_target_.add("gif", "wrongg");
    test_target_.add("css", "text/css");
    test_target_.add("gif", "image/gif");
    test_target_.add("jpg", "image/jpeg");
    test_target_.add("jpeg", "image/jpeg");
    test_target_.add("js", "application/x-javascript");
    test_target_.add("rss", "text/xml");
    test_target_.add("xml", "text/xml");
    test_target_.add("shtml", "text/html");
    test_target_.add("png", "image/png");
  }
  void TearDown() override {}
};

class TestDirectiveRedirect :
  public ::testing::TestWithParam<std::pair<std::string, bool> >
{
protected:
  configuration::DirectiveRedirect test_target_;
  void SetUp() override {}
  void TearDown() override {}
};

class TestDirectiveCgi :
  public ::testing::TestWithParam<std::pair<std::string, std::string> >
{
protected:
  configuration::DirectiveCgi test_target_;
  void SetUp() override {}
  void TearDown() override {}
};
