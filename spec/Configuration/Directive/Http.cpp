#include "./Blocks.hpp"

#include <gtest/gtest.h>

TEST(TestDirectiveHttp, constructor)
{
  directive::HttpBlock directive;
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveHttp);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveHttp, constructor2)
{
  directive::HttpBlock directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveHttp);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveHttp, constructor_copy)
{
  directive::HttpBlock directive(Directive::Context(50));
  directive::HttpBlock directive2(directive);
  ASSERT_EQ(directive2.is_block(), true);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveHttp);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}