#include "./Blocks.hpp"

#include <gtest/gtest.h>

#include "./Simple.hpp"

using namespace configuration;

TEST_F(TestDirectiveEvents, constructor)
{
  DirectiveEvents directive;
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveEvents);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST_F(TestDirectiveEvents, constructor2)
{
  DirectiveEvents directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveEvents);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST_F(TestDirectiveEvents, constructor_copy)
{
  DirectiveEvents directive(Directive::Context(50));
  DirectiveEvents directive2(directive);
  ASSERT_EQ(directive2.is_block(), true);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveEvents);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST_F(TestDirectiveEvents, query_directive_empty)
{
  configuration::DirectivesRange range = test_target_.query_directive(configuration::Directive::kDirectiveWorkerConnections);
  ASSERT_TRUE(range.first == range.second);
}

TEST_F(TestDirectiveEvents, query_directive_wrong)
{
  DirectiveWorkerConnections  worker_connections;
  worker_connections.set(1000);
  test_target_.add_directive(&worker_connections);
  configuration::DirectivesRange range = test_target_.query_directive(configuration::Directive::kDirectiveAccessLog);
  ASSERT_TRUE(range.first == range.second);
}

TEST_F(TestDirectiveEvents, query_directive)
{
  DirectiveWorkerConnections  worker_connections;
  worker_connections.set(1000);
  test_target_.add_directive(&worker_connections);
  configuration::DirectivesRange range = test_target_.query_directive(configuration::Directive::kDirectiveWorkerConnections);
  ASSERT_TRUE(range.first != range.second);
  ASSERT_EQ(range.first->second->type(), Directive::kDirectiveWorkerConnections);
  ASSERT_EQ(static_cast<DirectiveWorkerConnections*>(range.first->second)->get(), 1000);
  ASSERT_EQ(range.second, ++range.first);
}

TEST_F(TestDirectiveEvents, worker_connections_empty)
{
  ASSERT_EQ(test_target_.worker_connections().is_ok(), false);
}

TEST_F(TestDirectiveEvents, worker_connections)
{
  DirectiveWorkerConnections  worker_connections;
  worker_connections.set(1000);
  test_target_.add_directive(&worker_connections);
  ASSERT_EQ(test_target_.worker_connections().is_ok(), true);
  ASSERT_EQ(test_target_.worker_connections().value(), 1000);
}
