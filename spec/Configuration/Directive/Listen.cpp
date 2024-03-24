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
  using IpAddresses = std::vector<DirectiveListen::IpAddress>;
  IpAddresses ip_addresses = GetParam();
  
  for (IpAddresses::const_iterator it = ip_addresses.begin(); it != ip_addresses.end(); ++it)
  {
    test_target_.add(*it);
  }

  IpAddresses::const_iterator it = ip_addresses.begin();
  IpAddresses::const_iterator target_it = test_target_.get().begin();

  while (it != ip_addresses.end())
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
  using IpAddresses = std::vector<DirectiveListen::IpAddress>;
  DirectiveListen directive;
  IpAddresses ip_addresses = GetParam();
  
  for (IpAddresses::const_iterator it = ip_addresses.begin(); it != ip_addresses.end(); ++it)
  {
    directive.add(*it);
  }

  test_target_ = directive;

  IpAddresses::const_iterator it = ip_addresses.begin();
  IpAddresses::const_iterator target_it = test_target_.get().begin();

  while (it != ip_addresses.end())
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
  using IpAddresses = std::vector<DirectiveListen::IpAddress>;
  IpAddresses ip_addresses = GetParam();
  
  for (IpAddresses::const_iterator it = ip_addresses.begin(); it != ip_addresses.end(); ++it)
  {
    test_target_.add(*it);
  }

  DirectiveListen directive(test_target_);

  IpAddresses::const_iterator it = ip_addresses.begin();
  IpAddresses::const_iterator target_it = directive.get().begin();

  while (it != ip_addresses.end())
  {
    ASSERT_EQ(*it, *target_it);
    ++it;
    ++target_it;
  }
  // Test if the target iterator is at the end
  ASSERT_EQ(target_it, directive.get().end());
}

INSTANTIATE_TEST_SUITE_P(IpAddresses, TestDirectiveListen, testing::Values(
  std::vector<DirectiveListen::IpAddress>(),
	std::vector<DirectiveListen::IpAddress>{
    std::make_pair("", ""),
    std::make_pair("", ""),
    std::make_pair("", "")
  },
	std::vector<DirectiveListen::IpAddress>{
    std::make_pair("0.0.0.0", "80"),
    std::make_pair("127.27.34.52", "8080")
  },
	std::vector<DirectiveListen::IpAddress>{
    std::make_pair("192.27.34.52", "8080"),
    std::make_pair("34.128.55.52", "443"),
    std::make_pair("0.0.0.0", "80")
  }
));
