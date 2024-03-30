#include "./Simple.hpp"

#include <gtest/gtest.h>

TEST(TestDirectiveAllowMethods, constructor)
{
  directive::AllowMethods directive;
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveAllowMethods);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveAllowMethods, constructor2)
{
  directive::AllowMethods directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveAllowMethods);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveAllowMethods, constructor_copy)
{
  directive::AllowMethods directive(Directive::Context(50));
  directive::AllowMethods directive2(directive);
  ASSERT_EQ(directive2.is_block(), false);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveAllowMethods);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST(TestDirectiveAllowMethods, set_context)
{
  directive::AllowMethods directive;
  directive.set_context(Directive::Context(2));
  ASSERT_EQ(directive.context(), Directive::Context(2));
  ASSERT_EQ(directive.index(), 2);
}

TEST(TestDirectiveAllowMethods, set)
{
  directive::AllowMethods directive;
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
  if (methods & directive::Method::kMethodGet)
  {
    ASSERT_EQ(test_target_.is_allowed(directive::Method::kMethodGet), true);
  }
  if (methods & directive::Method::kMethodPost)
  {
    ASSERT_EQ(test_target_.is_allowed(directive::Method::kMethodPost), true);
  }
  if (methods & directive::Method::kMethodDelete)
  {
    ASSERT_EQ(test_target_.is_allowed(directive::Method::kMethodDelete), true);
  }
}

TEST_P(TestDirectiveAllowMethods, get)
{
  int methods = GetParam();

  test_target_.set(methods);
  directive::Methods result = test_target_.get();
  if (methods & directive::Method::kMethodGet)
  {
    ASSERT_EQ(result[0], true);
  }
  if (methods & directive::Method::kMethodPost)
  {
    ASSERT_EQ(result[1], true);
  }
  if (methods & directive::Method::kMethodDelete)
  {
    ASSERT_EQ(result[2], true);
  }
}

INSTANTIATE_TEST_SUITE_P(Methods, TestDirectiveAllowMethods, testing::Values(
  directive::Method::kMethodGet,
  directive::Method::kMethodPost,
  directive::Method::kMethodDelete,
  directive::Method::kMethodGet | directive::Method::kMethodPost,
  directive::Method::kMethodGet | directive::Method::kMethodDelete,
  directive::Method::kMethodPost | directive::Method::kMethodDelete,
  directive::Method::kMethodGet | directive::Method::kMethodPost | directive::Method::kMethodDelete
));
