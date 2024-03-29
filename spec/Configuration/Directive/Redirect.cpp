#include "./Simple.hpp"

#include <gtest/gtest.h>

TEST(TestDirectiveRedirect, constructor)
{
  directive::Redirect directive;
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveRedirect);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveRedirect, constructor2)
{
  directive::Redirect directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveRedirect);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveRedirect, constructor_copy)
{
  directive::Redirect directive(Directive::Context(50));
  directive::Redirect directive2(directive);
  ASSERT_EQ(directive2.is_block(), false);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveRedirect);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST(TestDirectiveRedirect, set_context)
{
  directive::Redirect directive;
  directive.set_context(Directive::Context(2));
  ASSERT_EQ(directive.context(), Directive::Context(2));
  ASSERT_EQ(directive.index(), 2);
}

TEST_P(TestDirectiveRedirect, getters_setters)
{
  std::pair<std::string, bool> redirect = GetParam();
  
  test_target_.set(redirect.first, redirect.second);
  ASSERT_EQ(test_target_.get_path(), redirect.first);
  ASSERT_EQ(test_target_.is_permanent(), redirect.second);
}

INSTANTIATE_TEST_SUITE_P(redirects, TestDirectiveRedirect, testing::Values(
  std::make_pair("", false),
  std::make_pair("", true),
  std::make_pair("/new", true),
  std::make_pair("/old/asd", true),
  std::make_pair("192.30.20.45:8000", true),
  std::make_pair("www.google.com", true),
  std::make_pair("www.google.com", false)
));
