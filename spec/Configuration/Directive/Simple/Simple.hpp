#pragma once

#include <gtest/gtest.h>

#include "Configuration/Directive/Socket.hpp"
#include "Configuration/Directive/Simple.hpp"
#include "Configuration/Directive/Simple/AllowMethods.hpp"
#include "Configuration/Directive/Simple/Cgi.hpp"
#include "Configuration/Directive/Simple/Listen.hpp"
#include "Configuration/Directive/Simple/MimeTypes.hpp"
#include "Configuration/Directive/Simple/Return.hpp"
#include "Configuration/Directive/Simple/ServerName.hpp"
#include "Configuration/Directive/Simple/ErrorPage.hpp"

/**
 * [/] Listen
 * [/] ServerName
 * [/] AllowMethods
 * [S] Root
 * [S] Index
 * [/] MimeTypes
 * [S] ErrorPage
 * [S] ClientMaxBodySize
 * [/] Return
 * [S] Autoindex
 * [/] Cgi
 * [S] AccessLog
 * [S] ErrorLog
 * [S] Include
 * [S] WorkerConnections
 * 
 * classes marked with S are tested in the TestDirectiveSimple typed tests.
*/

class TestDirectiveListen :
  public ::testing::TestWithParam<std::vector<directive::Socket> >
{
protected:
  directive::Listen test_target_;
};

class TestDirectiveServerName :
  public ::testing::TestWithParam<std::vector<std::string> >
{
protected:
  directive::ServerName test_target_;
};

class TestDirectiveAllowMethods :
  public ::testing::TestWithParam<int>
{
protected:
  directive::AllowMethods test_target_;
};

using MimeTypeVector = std::vector<std::pair<std::string, std::string> >;

class TestDirectiveMimeTypes :
  public ::testing::TestWithParam<MimeTypeVector>
{
protected:
  directive::MimeTypes test_target_;
};

class TestFilledDirectiveMimeTypes : public ::testing::Test
{
protected:
  directive::MimeTypes test_target_;
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

class TestDirectiveReturn :
  public ::testing::TestWithParam<std::pair<std::string, int> >
{
protected:
  directive::Return test_target_;
};

class TestDirectiveCgi :
  public ::testing::TestWithParam<std::pair<std::string, std::string> >
{
protected:
  directive::Cgi test_target_;
};

class TestDirectiveErrorPage :
  public ::testing::TestWithParam<std::pair<int, std::string> >
{
protected:
  directive::ErrorPage test_target_;
};

template <typename T>
class TestDirectiveSimple : public ::testing::Test
{
protected:
  std::vector<Directive*> test_targets_;

  void TearDown() override
  {
    for (auto& test_target : test_targets_)
      delete test_target;
  }
};
