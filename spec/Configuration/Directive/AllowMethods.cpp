#include "./Simple.hpp"

#include <gtest/gtest.h>

using namespace configuration;

TEST(TestDirectiveAllowMethods, constructor)
{
  DirectiveAllowMethods directive;
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveAllowMethods);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveAllowMethods, constructor2)
{
  DirectiveAllowMethods directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveAllowMethods);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveAllowMethods, constructor_copy)
{
  DirectiveAllowMethods directive(Directive::Context(50));
  DirectiveAllowMethods directive2(directive);
  ASSERT_EQ(directive2.is_block(), false);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveAllowMethods);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST(TestDirectiveAllowMethods, set_context)
{
  DirectiveAllowMethods directive;
  directive.set_context(Directive::Context(2));
  ASSERT_EQ(directive.context(), Directive::Context(2));
  ASSERT_EQ(directive.index(), 2);
}

TEST(TestDirectiveAllowMethods, set)
{
  DirectiveAllowMethods directive;
  EXPECT_DEATH({
    directive.set(-1);
  }, "Assertion.*");
  EXPECT_DEATH({
    directive.set(8);
  }, "Assertion.*");
}

TEST_P(TestDirectiveAllowMethods, is_allowed)
{
  int methods = GetParam();

  test_target_.set(methods);
  if (methods & Method::kMethodGet)
  {
    ASSERT_EQ(test_target_.is_allowed(Method::kMethodGet), true);
  }
  if (methods & Method::kMethodPost)
  {
    ASSERT_EQ(test_target_.is_allowed(Method::kMethodPost), true);
  }
  if (methods & Method::kMethodDelete)
  {
    ASSERT_EQ(test_target_.is_allowed(Method::kMethodDelete), true);
  }
}

TEST_P(TestDirectiveAllowMethods, get)
{
  int methods = GetParam();

  test_target_.set(methods);
  Methods result = test_target_.get();
  if (methods & Method::kMethodGet)
  {
    ASSERT_EQ(result[0], true);
  }
  if (methods & Method::kMethodPost)
  {
    ASSERT_EQ(result[1], true);
  }
  if (methods & Method::kMethodDelete)
  {
    ASSERT_EQ(result[2], true);
  }
}

INSTANTIATE_TEST_SUITE_P(Methods, TestDirectiveAllowMethods, testing::Values(
  Method::kMethodGet,
  Method::kMethodPost,
  Method::kMethodDelete,
  Method::kMethodGet | Method::kMethodPost,
  Method::kMethodGet | Method::kMethodDelete,
  Method::kMethodPost | Method::kMethodDelete,
  Method::kMethodGet | Method::kMethodPost | Method::kMethodDelete
));
