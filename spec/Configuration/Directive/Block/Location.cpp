#include "./Blocks.hpp"

#include <gtest/gtest.h>

TEST(TestDirectiveLocation, constructor)
{
  directive::LocationBlock directive;
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveLocation);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveLocation, constructor2)
{
  directive::LocationBlock directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), true);
  ASSERT_EQ(directive.type(), Directive::kDirectiveLocation);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveLocation, constructor_copy)
{
  directive::LocationBlock directive(Directive::Context(50));
  directive::LocationBlock directive2(directive);
  ASSERT_EQ(directive2.is_block(), true);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveLocation);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST(TestDirectiveLocation, best_match_use_first_encountered_location)
{
  directive::LocationBlock directive;
  
  directive.set("/");

  {
    directive::LocationBlock*  directive1 = new directive::LocationBlock();
    directive1->set("/");
    directive.add_directive(directive1);

    ASSERT_EQ(directive.best_match("/"), &directive);
  }
}

TEST(TestDirectiveLocation, best_match_use_first_encountered_location2)
{
  directive::LocationBlock directive;
  
  directive.set("/");

  {
    directive::LocationBlock*  directive1 = new directive::LocationBlock();
    directive1->set("/image");
    directive.add_directive(directive1);

    directive::LocationBlock*  directive2 = new directive::LocationBlock();
    directive2->set("/image");
    directive.add_directive(directive2);

    ASSERT_EQ(directive.best_match("/image/hi.html"), directive1);
  }
}

TEST(TestDirectiveLocation, best_match_use_longest_match)
{
  directive::LocationBlock directive;
  
  directive.set("/");

  {
    directive::LocationBlock*  directive1 = new directive::LocationBlock();
    directive1->set("/first");
    directive.add_directive(directive1);

    directive::LocationBlock*  directive2 = new directive::LocationBlock();
    directive2->set("/first/second");
    directive.add_directive(directive2);

    directive::LocationBlock*  directive3 = new directive::LocationBlock();
    directive3->set("/first/second/third");
    directive.add_directive(directive3);

    directive::LocationBlock*  directive4 = new directive::LocationBlock();
    directive4->set("/first/second/third/forth");
    directive.add_directive(directive4);

    ASSERT_EQ(directive.best_match("/first/second/third/forth/aye.html"), directive4);
  }
}
