#include "./Parser.hpp"

#include <gtest/gtest.h>

#include "Http/Parser.hpp"

static void  StringSliceIsEqualTest(http_parser::StringSlice string_slice, const char* start, StringSliceIndex string_slice_index)
{
  ASSERT_EQ(string_slice.length, string_slice_index.length);
  ASSERT_EQ(string_slice.bytes, start + string_slice_index.start);
}

TEST_P(TestParseToken, ParseToken)
{
  ParseTestCase params = GetParam();
  http_parser::StringSlice  input(std::get<0>(params).c_str(), std::get<0>(params).length());
  http_parser::ParseOutput output = http_parser::ParseToken(input);
  ASSERT_EQ(output.status, std::get<1>(params));
  StringSliceIsEqualTest(output.rest, input.bytes, std::get<2>(params));
  ASSERT_EQ(output.parsed_length, std::get<3>(params));
  Maybe<StringSliceIndex> string_slice_index = std::get<4>(params);
  if (string_slice_index.is_ok())
  {
    ASSERT_TRUE(output.result != NULL);
    ASSERT_EQ(output.result->type, http_parser::kToken);
    StringSliceIsEqualTest(static_cast<http_parser::PTNodeToken*>(output.result)->content, input.bytes, string_slice_index.value());
  }
  else
  {
    ASSERT_TRUE(output.result == NULL);
  }
}

INSTANTIATE_TEST_SUITE_P(tokens, TestParseToken, testing::Values(
  std::make_tuple("", http_parser::kParseFailure, StringSliceIndex(0, 0), 0, Nothing())
));

// TEST_P(TestParsePath, operator_equal)
// {
//   ParseTestCase params = GetParam();
// }
//
// INSTANTIATE_TEST_SUITE_P(paths, TestParsePath, testing::Values(
//   std::make_tuple("0.0.0.0", "80", 1, "0.0.0.0"),
//   std::make_tuple("::", "80", 1, "::"),
//   std::make_tuple("0000:0000:0000:0000:0000:0000:0000:0001", "80", 1, "::1"),
//   std::make_tuple("2001:0db8:ab00:0000:0000:0000:0000:0000", "9000", 1, "2001:db8:ab00::"),
//   std::make_tuple("2001:0db8:c9d2:0012:0000:0000:0000:0051", "9000", 1, "2001:db8:c9d2:12::51"),
//   std::make_tuple("2001:0db8:c9d2:0000:0001:0000:0000:0051", "9000", 1, "2001:db8:c9d2:0:1::51"),
//   std::make_tuple("1:0:0001:0000:010:0000:0100:0000", "9000", 1, "1:0:1:0:10:0:100:0"),
//   std::make_tuple("0:0:0001:0000:000:0000:0100:0000", "9000", 1, "0:0:1::100:0")
// ));
