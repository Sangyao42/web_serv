#include "./Simple.hpp"

#include <gtest/gtest.h>

TEST(TestDirectiveErrorPage, constructor)
{
  directive::ErrorPage directive;
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveErrorPage);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveErrorPage, constructor2)
{
  directive::ErrorPage directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveErrorPage);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveErrorPage, constructor_copy)
{
  directive::ErrorPage directive(Directive::Context(50));
  directive::ErrorPage directive2(directive);
  ASSERT_EQ(directive2.is_block(), false);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveErrorPage);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST(TestDirectiveErrorPage, set_context)
{
  directive::ErrorPage directive;
  directive.set_context(Directive::Context(2));
  ASSERT_EQ(directive.context(), Directive::Context(2));
  ASSERT_EQ(directive.index(), 2);
}

TEST_P(TestDirectiveErrorPage, getters_setters)
{
  std::pair<int, std::string> redirect = GetParam();
  
  test_target_.set(redirect.first, redirect.second);
  ASSERT_EQ(test_target_.get(), redirect.first);
  ASSERT_EQ(test_target_.file_path(), redirect.second);
}

INSTANTIATE_TEST_SUITE_P(redirects, TestDirectiveErrorPage, testing::Values(
  std::make_pair(301, "/index.htm"),
  std::make_pair(307, "/index.json"),
  std::make_pair(200, "/new.html"),
  std::make_pair(200, "/old/asd.html"),
  std::make_pair(501, "/error.html"),
  std::make_pair(404, "/notfound.html"),
  std::make_pair(100, "")
));
