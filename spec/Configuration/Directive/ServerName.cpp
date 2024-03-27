#include "./Simple.hpp"

#include <gtest/gtest.h>

using namespace configuration;

TEST(TestDirectiveServerName, constructor)
{
  DirectiveServerName directive;
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveServerName);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveServerName, constructor2)
{
  DirectiveServerName directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveServerName);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveServerName, constructor_copy)
{
  DirectiveServerName directive(Directive::Context(50));
  DirectiveServerName directive2(directive);
  ASSERT_EQ(directive2.is_block(), false);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveServerName);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST(TestDirectiveServerName, set_context)
{
  DirectiveServerName directive;
  directive.set_context(Directive::Context(2));
  ASSERT_EQ(directive.context(), Directive::Context(2));
  ASSERT_EQ(directive.index(), 2);
}

TEST_P(TestDirectiveServerName, add)
{
  using ServerNames = std::vector<std::string>;
  ServerNames server_names = GetParam();
  
  for (ServerNames::const_iterator it = server_names.begin(); it != server_names.end(); ++it)
  {
    test_target_.add(*it);
  }

  ServerNames::const_iterator it = server_names.begin();
  ServerNames::const_iterator target_it = test_target_.get().begin();

  while (it != server_names.end())
  {
    ASSERT_EQ(*it, *target_it);
    ++it;
    ++target_it;
  }
  // Test if the target iterator is at the end
  ASSERT_EQ(target_it, test_target_.get().end());
}

TEST_P(TestDirectiveServerName, add_then_assignment)
{
  using ServerNames = std::vector<std::string>;
  DirectiveServerName directive;
  ServerNames server_names = GetParam();
  
  for (ServerNames::const_iterator it = server_names.begin(); it != server_names.end(); ++it)
  {
    directive.add(*it);
  }

  test_target_ = directive;

  ServerNames::const_iterator it = server_names.begin();
  ServerNames::const_iterator target_it = test_target_.get().begin();

  while (it != server_names.end())
  {
    ASSERT_EQ(*it, *target_it);
    ++it;
    ++target_it;
  }
  // Test if the target iterator is at the end
  ASSERT_EQ(target_it, test_target_.get().end());
}

TEST_P(TestDirectiveServerName, add_then_copy)
{
  using ServerNames = std::vector<std::string>;
  ServerNames server_names = GetParam();
  
  for (ServerNames::const_iterator it = server_names.begin(); it != server_names.end(); ++it)
  {
    test_target_.add(*it);
  }

  DirectiveServerName directive(test_target_);

  ServerNames::const_iterator it = server_names.begin();
  ServerNames::const_iterator target_it = directive.get().begin();

  while (it != server_names.end())
  {
    ASSERT_EQ(*it, *target_it);
    ++it;
    ++target_it;
  }
  // Test if the target iterator is at the end
  ASSERT_EQ(target_it, directive.get().end());
}

INSTANTIATE_TEST_SUITE_P(server_names, TestDirectiveServerName, testing::Values(
  std::vector<std::string>(),
	std::vector<std::string>{
    "",
    "",
    ""
  },
	std::vector<std::string>{
    "www.google.com",
    "google.com"
  },
	std::vector<std::string>{
    "hi.xyz",
    "what.hi.xyz",
    "what.xyz",
    "example.com",
    "www.example.com",
  }
));
