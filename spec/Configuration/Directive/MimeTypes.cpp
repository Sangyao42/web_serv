#include "./Simple.hpp"

#include <gtest/gtest.h>

using namespace configuration;

TEST(TestDirectiveMimeTypes, constructor)
{
  DirectiveMimeTypes directive;
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveMimeTypes);
  ASSERT_EQ(directive.context(), Directive::Context(0));
  ASSERT_EQ(directive.index(), 0);
}

TEST(TestDirectiveMimeTypes, constructor2)
{
  DirectiveMimeTypes directive(Directive::Context(50));
  ASSERT_EQ(directive.is_block(), false);
  ASSERT_EQ(directive.type(), Directive::kDirectiveMimeTypes);
  ASSERT_EQ(directive.context(), Directive::Context(50));
  ASSERT_EQ(directive.index(), 50);
}

TEST(TestDirectiveMimeTypes, constructor_copy)
{
  DirectiveMimeTypes directive(Directive::Context(50));
  DirectiveMimeTypes directive2(directive);
  ASSERT_EQ(directive2.is_block(), false);
  ASSERT_EQ(directive2.type(), Directive::kDirectiveMimeTypes);
  ASSERT_EQ(directive2.context(), Directive::Context(50));
  ASSERT_EQ(directive2.index(), 50);
}

TEST(TestDirectiveMimeTypes, set_context)
{
  DirectiveMimeTypes directive;
  directive.set_context(Directive::Context(2));
  ASSERT_EQ(directive.context(), Directive::Context(2));
  ASSERT_EQ(directive.index(), 2);
}

using MimeTypes = std::map<DirectiveMimeTypes::Extension, DirectiveMimeTypes::MimeType>;

TEST_P(TestDirectiveMimeTypes, add)
{
  MimeTypeVector mime_types_vec = GetParam();

  for (MimeTypeVector::const_iterator it = mime_types_vec.begin(); it != mime_types_vec.end(); ++it)
  {
    test_target_.add(it->first, it->second);
  }

  const MimeTypes& mime_types = test_target_.get();

  for (MimeTypes::const_iterator it = mime_types.begin(); it != mime_types.end(); ++it)
  {
    // find the last occurrence of the extension
    DirectiveMimeTypes::MimeType mime_type;
    for (MimeTypeVector::const_iterator it2 = mime_types_vec.begin(); it2 != mime_types_vec.end(); ++it2)
    {
      if (it2->first == it->first)
      {
        mime_type = it2->second;
      }
    }
    ASSERT_EQ(it->second, mime_type);
  }
}

TEST_P(TestDirectiveMimeTypes, add_then_assignment)
{
  DirectiveMimeTypes directive;
  MimeTypeVector mime_types_vec = GetParam();

  for (MimeTypeVector::const_iterator it = mime_types_vec.begin(); it != mime_types_vec.end(); ++it)
  {
    directive.add(it->first, it->second);
  }

  test_target_ = directive;
  const MimeTypes& mime_types = test_target_.get();

  for (MimeTypes::const_iterator it = mime_types.begin(); it != mime_types.end(); ++it)
  {
    // find the last occurrence of the extension
    DirectiveMimeTypes::MimeType mime_type;
    for (MimeTypeVector::const_iterator it2 = mime_types_vec.begin(); it2 != mime_types_vec.end(); ++it2)
    {
      if (it2->first == it->first)
      {
        mime_type = it2->second;
      }
    }
    ASSERT_EQ(it->second, mime_type);
  }
}

TEST_P(TestDirectiveMimeTypes, add_then_copy)
{
  MimeTypeVector mime_types_vec = GetParam();

  for (MimeTypeVector::const_iterator it = mime_types_vec.begin(); it != mime_types_vec.end(); ++it)
  {
    test_target_.add(it->first, it->second);
  }

  DirectiveMimeTypes directive(test_target_);
  const MimeTypes& mime_types = directive.get();

  for (MimeTypes::const_iterator it = mime_types.begin(); it != mime_types.end(); ++it)
  {
    // find the last occurrence of the extension
    DirectiveMimeTypes::MimeType mime_type;
    for (MimeTypeVector::const_iterator it2 = mime_types_vec.begin(); it2 != mime_types_vec.end(); ++it2)
    {
      if (it2->first == it->first)
      {
        mime_type = it2->second;
      }
    }
    ASSERT_EQ(it->second, mime_type);
  }
}

TEST_F(TestFilledDirectiveMimeTypes, query)
{
  ASSERT_EQ(test_target_.query("html").value(), "text/html");
  ASSERT_EQ(test_target_.query("htm").value(), "text/html");
  ASSERT_EQ(test_target_.query("shtml").value(), "text/html");
  ASSERT_EQ(test_target_.query("css").value(), "text/css");
  ASSERT_EQ(test_target_.query("xml").value(), "text/xml");
  ASSERT_EQ(test_target_.query("rss").value(), "text/xml");
  ASSERT_EQ(test_target_.query("js").value(), "application/x-javascript");
  ASSERT_EQ(test_target_.query("jpeg").value(), "image/jpeg");
  ASSERT_EQ(test_target_.query("jpg").value(), "image/jpeg");
}

TEST_F(TestFilledDirectiveMimeTypes, query_repeated)
{
  ASSERT_EQ(test_target_.query("gif").value(), "image/gif");
}

TEST_F(TestFilledDirectiveMimeTypes, ignore_empty_entry)
{
  ASSERT_EQ(test_target_.get().size(), static_cast<size_t>(11));
}

TEST_F(TestFilledDirectiveMimeTypes, query_incorrect_key)
{
  ASSERT_EQ(test_target_.query("wrong").is_ok(), false);
}

INSTANTIATE_TEST_SUITE_P(mime_types, TestDirectiveMimeTypes, testing::Values(
  MimeTypeVector(),
	MimeTypeVector{
    std::make_pair("", ""),
    std::make_pair("", ""),
    std::make_pair("", "")
  },
	MimeTypeVector{
    std::make_pair("gif", "text/html"),
    std::make_pair("gif", "text/css"),
    std::make_pair("gif", "image/gif")
  },
	MimeTypeVector{
    std::make_pair("html", "text/html"),
    std::make_pair("htm", "text/html"),
    std::make_pair("shtml", "text/html"),
    std::make_pair("css", "text/css"),
    std::make_pair("xml", "text/xml"),
    std::make_pair("rss", "text/xml"),
    std::make_pair("js", "application/x-javascript"),
    std::make_pair("jpeg", "image/jpeg"),
    std::make_pair("jpg", "image/jpeg"),
    std::make_pair("png", "image/png")
  }
));
