#include "./Blocks.hpp"

#include <gtest/gtest.h>

using namespace configuration;

TEST(TestDirectiveEvents, constructor)
{
  DirectiveEvents directive;
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveEvents);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveEvents, constructor2)
{
  DirectiveEvents directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveEvents);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveEvents, constructor_copy)
{
  DirectiveEvents directive(Directive::Context(50));
  DirectiveEvents directive2(directive);
  ASSERT_EQ(directive2.is_block(), true);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveEvents);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}