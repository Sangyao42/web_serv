#include "./Simple.hpp"

#include <gtest/gtest.h>

using TestTypes = ::testing::Types<\
  std::string,
  bool,
  Directive*,
  size_t
>;

TYPED_TEST_SUITE(TestDirectiveSimple, TestTypes, );

static const std::vector<Directive::Type> simple_directive_names = {
  Directive::kDirectiveRoot,
  Directive::kDirectiveIndex,
  Directive::kDirectiveErrorPage,
  Directive::kDirectiveClientMaxBodySize,
  Directive::kDirectiveAutoindex,
  Directive::kDirectiveAccessLog,
  Directive::kDirectiveErrorLog,
  Directive::kDirectiveInclude,
  Directive::kDirectiveWorkerConnections
};

TYPED_TEST(TestDirectiveSimple, constructor)
{
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveRoot>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveIndex>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveErrorPage>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveClientMaxBodySize>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveAutoindex>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveAccessLog>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveErrorLog>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveInclude>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveWorkerConnections>());
  std::vector<Directive::Type>::const_iterator it = simple_directive_names.begin();
  for (auto& test_target : this->test_targets_)
  {
    ASSERT_EQ(test_target->is_block(), false);
    ASSERT_EQ(test_target->type(), *it);
    ASSERT_EQ(test_target->context(), Directive::Context(0));
    ASSERT_EQ(test_target->index(), 0);
    it++;
  }
  ASSERT_TRUE(it == simple_directive_names.end());
}

TYPED_TEST(TestDirectiveSimple, constructor2)
{
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveRoot>(50));
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveIndex>(50));
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveErrorPage>(50));
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveClientMaxBodySize>(50));
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveAutoindex>(50));
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveAccessLog>(50));
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveErrorLog>(50));
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveInclude>(50));
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveWorkerConnections>(50));
  std::vector<Directive::Type>::const_iterator it = simple_directive_names.begin();
  for (auto& test_target : this->test_targets_)
  {
    ASSERT_EQ(test_target->is_block(), false);
    ASSERT_EQ(test_target->type(), *it);
    ASSERT_EQ(test_target->context(), Directive::Context(50));
    ASSERT_EQ(test_target->index(), 50);
    it++;
  }
  ASSERT_TRUE(it == simple_directive_names.end());
}

TYPED_TEST(TestDirectiveSimple, set_context)
{
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveRoot>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveIndex>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveErrorPage>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveClientMaxBodySize>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveAutoindex>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveAccessLog>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveErrorLog>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveInclude>());
  this->test_targets_.push_back(new directive::DirectiveSimple<TypeParam, Directive::kDirectiveWorkerConnections>());
  
  for (auto& test_target : this->test_targets_)
  {
    test_target->set_context(Directive::Context(2));
  }

  std::vector<Directive::Type>::const_iterator it = simple_directive_names.begin();
  for (auto& test_target : this->test_targets_)
  {
    ASSERT_EQ(test_target->is_block(), false);
    ASSERT_EQ(test_target->type(), *it);
    ASSERT_EQ(test_target->context(), Directive::Context(2));
    ASSERT_EQ(test_target->index(), 2);
    it++;
  }
  ASSERT_TRUE(it == simple_directive_names.end());
}
