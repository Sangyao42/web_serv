#include "./Blocks.hpp"

#include <gtest/gtest.h>

TEST(TestDirectiveMain, constructor)
{
  directive::MainBlock directive;
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveMain);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveMain, constructor2)
{
  directive::MainBlock directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveMain);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveMain, constructor_copy)
{
  directive::MainBlock directive(Directive::Context(50));
  directive::MainBlock directive2(directive);
  ASSERT_EQ(directive2.is_block(), true);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveMain);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}
