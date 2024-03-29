#include "./Blocks.hpp"

#include <gtest/gtest.h>

#include "./Simple.hpp"

TEST_F(TestDirectiveEvents, constructor)
{
  directive::EventsBlock directive;
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveEvents);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST_F(TestDirectiveEvents, constructor2)
{
  directive::EventsBlock directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveEvents);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST_F(TestDirectiveEvents, constructor_copy)
{
  directive::EventsBlock directive(Directive::Context(50));
  directive::EventsBlock directive2(directive);
  ASSERT_EQ(directive2.is_block(), true);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveEvents);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST_F(TestDirectiveEvents, query_directive_empty)
{
  directive::DirectivesRange range = test_target_.query_directive(Directive::kDirectiveWorkerConnections);
  ASSERT_TRUE(range.first == range.second);
}

TEST_F(TestDirectiveEvents, query_directive_wrong)
{
  directive::WorkerConnections  worker_connections;
  worker_connections.set(1000);
  test_target_.add_directive(&worker_connections);
  directive::DirectivesRange range = test_target_.query_directive(Directive::kDirectiveAccessLog);
  ASSERT_TRUE(range.first == range.second);
}

TEST_F(TestDirectiveEvents, query_directive)
{
  directive::WorkerConnections  worker_connections;
  worker_connections.set(1000);
  test_target_.add_directive(&worker_connections);
  directive::DirectivesRange range = test_target_.query_directive(Directive::kDirectiveWorkerConnections);
  ASSERT_TRUE(range.first != range.second);
  ASSERT_EQ(range.first->second->type(), Directive::kDirectiveWorkerConnections);
  ASSERT_EQ(static_cast<directive::WorkerConnections*>(range.first->second)->get(), static_cast<size_t>(1000));
  ASSERT_EQ(range.second, ++range.first);
}

TEST_F(TestDirectiveEvents, worker_connections_empty)
{
  ASSERT_EQ(test_target_.worker_connections().is_ok(), false);
}

TEST_F(TestDirectiveEvents, worker_connections)
{
  directive::WorkerConnections  worker_connections;
  worker_connections.set(1000);
  test_target_.add_directive(&worker_connections);
  ASSERT_EQ(test_target_.worker_connections().is_ok(), true);
  ASSERT_EQ(test_target_.worker_connections().value(), static_cast<size_t>(1000));
}
