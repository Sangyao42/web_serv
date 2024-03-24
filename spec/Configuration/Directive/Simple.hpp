#pragma once

#include <gtest/gtest.h>

#include "Configuration/Directive/Simple.hpp"

/**
 * [/] DirectiveListen
 * [/] DirectiveServerName
 * [/] DirectiveAllowMethods
 * [S] DirectiveRoot
 * [S] DirectiveIndex
 * [/] DirectiveMimeTypes
 * [S] DirectiveErrorPage
 * [S] DirectiveClientMaxBodySize
 * [/] DirectiveRedirect
 * [S] DirectiveAutoindex
 * [/] DirectiveCgi
 * [S] DirectiveAccessLog
 * [S] DirectiveErrorLog
 * [S] DirectiveInclude
 * [S] DirectiveWorkerConnections
 * 
 * classes marked with S are tested in the TestDirectiveSimple typed tests.
*/

class TestDirectiveListen :
  public ::testing::TestWithParam<std::vector<configuration::DirectiveListen::IpAddress> >
{
protected:
  configuration::DirectiveListen test_target_;
};

class TestDirectiveServerName :
  public ::testing::TestWithParam<std::vector<std::string> >
{
protected:
  configuration::DirectiveServerName test_target_;
};

class TestDirectiveAllowMethods :
  public ::testing::TestWithParam<int>
{
protected:
  configuration::DirectiveAllowMethods test_target_;
};

using MimeTypeVector = std::vector<std::pair<std::string, std::string> >;

class TestDirectiveMimeTypes :
  public ::testing::TestWithParam<MimeTypeVector>
{
protected:
  configuration::DirectiveMimeTypes test_target_;
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
};

class TestDirectiveRedirect :
  public ::testing::TestWithParam<std::pair<std::string, bool> >
{
protected:
  configuration::DirectiveRedirect test_target_;
};

class TestDirectiveCgi :
  public ::testing::TestWithParam<std::pair<std::string, std::string> >
{
protected:
  configuration::DirectiveCgi test_target_;
};

template <typename T>
class TestDirectiveSimple : public ::testing::Test
{
protected:
  std::vector<configuration::Directive*> test_targets_;

  void TearDown() override
  {
    for (auto& test_target : test_targets_)
      delete test_target;
  }
};
