#include "./Simple.hpp"

#include <gtest/gtest.h>

TEST(TestDirectiveReturn, constructor)
{
  directive::Return directive;
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveReturn);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveReturn, constructor2)
{
  directive::Return directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveReturn);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveReturn, constructor_copy)
{
  directive::Return directive(Directive::Context(50));
  directive::Return directive2(directive);
  ASSERT_EQ(directive2.is_block(), false);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveReturn);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST(TestDirectiveReturn, set_context)
{
  directive::Return directive;
  directive.set_context(Directive::Context(2));
  ASSERT_EQ(directive.context(), Directive::Context(2));
  ASSERT_EQ(directive.index(), 2);
}

TEST_P(TestDirectiveReturn, getters_setters)
{
  std::pair<std::string, bool> redirect = GetParam();
  
  test_target_.set(redirect.first, redirect.second);
  ASSERT_EQ(test_target_.get_path(), redirect.first);
  ASSERT_EQ(test_target_.status_code(), redirect.second);
}

INSTANTIATE_TEST_SUITE_P(redirects, TestDirectiveReturn, testing::Values(
  std::make_pair("", 301),
  std::make_pair("", 302),
  std::make_pair("/new", 200),
  std::make_pair("/old/asd", 200),
  std::make_pair("192.30.20.45:8000", 307),
  std::make_pair("www.google.com", 302),
  std::make_pair("www.google.com", 100)
));
