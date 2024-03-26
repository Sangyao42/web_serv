#include "./Simple.hpp"

#include <gtest/gtest.h>

using namespace configuration;

TEST(TestDirectiveListen, constructor)
{
  DirectiveListen directive;
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveListen);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveListen, constructor2)
{
  DirectiveListen directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveListen);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveListen, constructor_copy)
{
  DirectiveListen directive(Directive::Context(50));
  DirectiveListen directive2(directive);
  ASSERT_EQ(directive2.is_block(), false);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveListen);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST(TestDirectiveListen, set_context)
{
  DirectiveListen directive;
  directive.set_context(Directive::Context(2));
  ASSERT_EQ(directive.context(), Directive::Context(2));
  ASSERT_EQ(directive.index(), 2);
}

TEST_P(TestDirectiveListen, add)
{
  using Sockets = std::vector<Socket>;
  Sockets sockets = GetParam();
  
  for (Sockets::const_iterator it = sockets.begin(); it != sockets.end(); ++it)
  {
    test_target_.add(*it);
  }

  Sockets::const_iterator it = sockets.begin();
  Sockets::const_iterator target_it = test_target_.get().begin();

  while (it != sockets.end())
  {
    ASSERT_EQ(*it, *target_it);
    ++it;
    ++target_it;
  }
  // Test if the target iterator is at the end
  ASSERT_EQ(target_it, test_target_.get().end());
}

TEST_P(TestDirectiveListen, add_then_assignment)
{
  using Sockets = std::vector<Socket>;
  DirectiveListen directive;
  Sockets sockets = GetParam();
  
  for (Sockets::const_iterator it = sockets.begin(); it != sockets.end(); ++it)
  {
    directive.add(*it);
  }

  test_target_ = directive;

  Sockets::const_iterator it = sockets.begin();
  Sockets::const_iterator target_it = test_target_.get().begin();

  while (it != sockets.end())
  {
    ASSERT_EQ(*it, *target_it);
    ++it;
    ++target_it;
  }
  // Test if the target iterator is at the end
  ASSERT_EQ(target_it, test_target_.get().end());
}

TEST_P(TestDirectiveListen, add_then_copy)
{
  using Sockets = std::vector<Socket>;
  Sockets sockets = GetParam();
  
  for (Sockets::const_iterator it = sockets.begin(); it != sockets.end(); ++it)
  {
    test_target_.add(*it);
  }

  DirectiveListen directive(test_target_);

  Sockets::const_iterator it = sockets.begin();
  Sockets::const_iterator target_it = directive.get().begin();

  while (it != sockets.end())
  {
    ASSERT_EQ(*it, *target_it);
    ++it;
    ++target_it;
  }
  // Test if the target iterator is at the end
  ASSERT_EQ(target_it, directive.get().end());
}

INSTANTIATE_TEST_SUITE_P(Sockets, TestDirectiveListen, testing::Values(
  std::vector<Socket>(),
	std::vector<Socket>{
    Socket(),
    Socket(),
    Socket()
  },
	std::vector<Socket>{
    Socket("0.0.0.0", "80"),
    Socket("127.27.34.52", "8080")
  },
	std::vector<Socket>{
    Socket("192.27.34.52", "8080"),
    Socket("34.128.55.52", "443"),
    Socket("0.0.0.0", "80")
  }
));
