#include "./Socket.hpp"

#include <gtest/gtest.h>
#include <sys/socket.h>

#include "Configuration/Directive/Socket.hpp"

TEST_P(TestConfigurationSocket, recognize_family)
{
  SocketTest params = GetParam();
  directive::Socket socket(std::get<0>(params), std::get<1>(params));

  EXPECT_EQ(socket.ip_address(), std::get<0>(params));
  EXPECT_EQ(socket.port(), std::get<1>(params));
  EXPECT_EQ(socket.family(), std::get<2>(params));
}

INSTANTIATE_TEST_SUITE_P(sockets, TestConfigurationSocket, testing::Values(
  std::make_tuple("0.0.0.0", "80", AF_INET),
  std::make_tuple("::", "80", AF_INET6),
  std::make_tuple("0000:0000:0000:0000:0000:0000:0000:0001", "80", AF_INET6),
  std::make_tuple("2001:0db8:ab00:0000:0000:0000:0000:0000", "9000", AF_INET6),
  std::make_tuple("2001:0db8:c9d2:0012:0000:0000:0000:0051", "9000", AF_INET6),
  std::make_tuple("2001:0db8:c9d2:0000:0001:0000:0000:0051", "9000", AF_INET6),
  std::make_tuple("1:0:0001:0000:010:0000:0100:0000", "9000", AF_INET6),
  std::make_tuple("0:0:0001:0000:000:0000:0100:0000", "9000", AF_INET6)
));

TEST_P(TestConfigurationSocketSerialized, operator_equal)
{
  SocketSerializedTest params = GetParam();
  directive::Socket socket(std::get<0>(params), std::get<1>(params));
  directive::Socket socket2(std::get<3>(params), std::get<1>(params));

  EXPECT_EQ(socket, socket2);
}

INSTANTIATE_TEST_SUITE_P(sockets, TestConfigurationSocketSerialized, testing::Values(
  std::make_tuple("0.0.0.0", "80", AF_INET, "0.0.0.0"),
  std::make_tuple("::", "80", AF_INET6, "::"),
  std::make_tuple("0000:0000:0000:0000:0000:0000:0000:0001", "80", AF_INET6, "::1"),
  std::make_tuple("2001:0db8:ab00:0000:0000:0000:0000:0000", "9000", AF_INET6, "2001:db8:ab00::"),
  std::make_tuple("2001:0db8:c9d2:0012:0000:0000:0000:0051", "9000", AF_INET6, "2001:db8:c9d2:12::51"),
  std::make_tuple("2001:0db8:c9d2:0000:0001:0000:0000:0051", "9000", AF_INET6, "2001:db8:c9d2:0:1::51"),
  std::make_tuple("1:0:0001:0000:010:0000:0100:0000", "9000", AF_INET6, "1:0:1:0:10:0:100:0"),
  std::make_tuple("0:0:0001:0000:000:0000:0100:0000", "9000", AF_INET6, "0:0:1::100:0")
));
