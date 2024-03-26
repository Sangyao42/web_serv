#pragma once

#include <gtest/gtest.h>

#include <string>
#include <tuple>

#include "Configuration/Socket.hpp"

using SocketTest = std::tuple<std::string, std::string, u_int16_t, std::string>;

class TestConfigurationSocket : public ::testing::TestWithParam<SocketTest> {};