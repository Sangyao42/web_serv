#pragma once

#include <gtest/gtest.h>

#include <string>
#include <tuple>

#include "misc/Maybe.hpp"
#include "Http/Parser.hpp"

struct StringSliceIndex
{
  int start;
  int length;

  StringSliceIndex()
    : start(0), length(0) {}
  StringSliceIndex(int _start, int _length)
    : start(_start), length(_length) {}
};

using ParseTestCase = std::tuple<std::string,
                                 http_parser::ParseOutputStatus,
                                 StringSliceIndex,
                                 int,
                                 Maybe<StringSliceIndex> >;

class TestParseToken : public ::testing::TestWithParam<ParseTestCase> {};

class TestParsePath : public ::testing::TestWithParam<ParseTestCase> {};

