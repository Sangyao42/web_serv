#include "./Simple.hpp"

#include <gtest/gtest.h>

TEST(TestDirectiveCgi, constructor)
{
  directive::Cgi directive;
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveCgi);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveCgi, constructor2)
{
  directive::Cgi directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveCgi);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveCgi, constructor_copy)
{
  directive::Cgi directive(Directive::Context(50));
  directive::Cgi directive2(directive);
  ASSERT_EQ(directive2.is_block(), false);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveCgi);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST(TestDirectiveCgi, set_context)
{
  directive::Cgi directive;
  directive.set_context(Directive::Context(2));
  ASSERT_EQ(directive.context(), Directive::Context(2));
  ASSERT_EQ(directive.index(), 2);
}

TEST_P(TestDirectiveCgi, match)
{
  std::pair<std::string, std::string> cgi_setting = GetParam();

  test_target_.set(cgi_setting.first, cgi_setting.second);
  ASSERT_EQ(test_target_.match(cgi_setting.first).is_ok(), true);
  ASSERT_EQ(test_target_.match(cgi_setting.first).value(), cgi_setting.second);
  ASSERT_EQ(test_target_.match("notfound").is_ok(), false);
}

INSTANTIATE_TEST_SUITE_P(Methods, TestDirectiveCgi, testing::Values(
  std::make_pair("", "/usr/bin/php-cgi"),
  std::make_pair("php", ""),
  std::make_pair("", ""),
  std::make_pair("php", "/usr/bin/php-cgi"),
  std::make_pair("py", "/usr/bin/python3"),
  std::make_pair("pl", "/usr/bin/perl")
));
