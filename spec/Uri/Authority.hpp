#pragma once

#include <gtest/gtest.h>

#include <string>
#include <tuple>

#include "Uri/Authority.hpp"

using UriAuthorityTestCase = std::tuple<std::string, std::string, u_int16_t>;
using uriAuthoritySerializedTestCase = std::tuple<std::string, std::string, u_int16_t, std::string>;

class TestUriAuthority : public ::testing::TestWithParam<UriAuthorityTestCase> {};

class TestUriAuthoritySerialized : public ::testing::TestWithParam<uriAuthoritySerializedTestCase> {};