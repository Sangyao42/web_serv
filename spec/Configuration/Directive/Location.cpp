#include "./Blocks.hpp"

#include <gtest/gtest.h>

using namespace configuration;

TEST(TestDirectiveLocation, constructor)
{
  DirectiveLocation directive;
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveLocation);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveLocation, constructor2)
{
  DirectiveLocation directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveLocation);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveLocation, constructor_copy)
{
  DirectiveLocation directive(Directive::Context(50));
  DirectiveLocation directive2(directive);
  ASSERT_EQ(directive2.is_block(), true);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveLocation);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}