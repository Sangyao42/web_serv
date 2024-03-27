#include <gtest/gtest.h>

#include "Configuration/Directive.hpp"

using namespace configuration;

TEST(TestDirectiveSimple, directive_context_constructor1)
{
  Directive::Context context;
  ASSERT_EQ(context.index, 0);
}

TEST(TestDirectiveSimple, directive_context_constructor2)
{
  Directive::Context context(13);
  ASSERT_EQ(context.index, 13);

  context = 15;
  ASSERT_EQ(context.index, 15);
}
