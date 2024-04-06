#include <gtest/gtest.h>

#include "constants.hpp"

#include "Configuration/Directive/Simple/AllowMethods.hpp"

TEST(Constants, kDefaultAllowedMethods)
{
  ASSERT_EQ(constants::kDefaultAllowedMethods, directive::kMethodGet | directive::kMethodDelete);
}

TEST(Constants, kDefaultMimeTypes)
{
  ASSERT_EQ(constants::kDefaultMimeTypes.query("html").is_ok(), true);
  ASSERT_EQ(constants::kDefaultMimeTypes.query("html").value(), "text/html");
  ASSERT_EQ(constants::kDefaultMimeTypes.query("gif").is_ok(), true);
  ASSERT_EQ(constants::kDefaultMimeTypes.query("gif").value(), "image/gif");
  ASSERT_EQ(constants::kDefaultMimeTypes.query("jpg").is_ok(), true);
  ASSERT_EQ(constants::kDefaultMimeTypes.query("jpg").value(), "image/jpeg");
  ASSERT_EQ(constants::kDefaultMimeTypes.get().size(), static_cast<size_t>(3));
}
