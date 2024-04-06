#include "./Authority.hpp"

#include <gtest/gtest.h>
#include <sys/socket.h>

#include "Uri/Authority.hpp"

TEST_P(TestUriAuthority, recognize_family)
{
  UriAuthorityTestCase params = GetParam();
  uri::Authority authority(std::get<0>(params), std::get<1>(params));

  EXPECT_EQ(authority.host.value, std::get<0>(params));
  EXPECT_EQ(authority.port, std::get<1>(params));
  EXPECT_EQ(authority.family(), std::get<2>(params));
}

INSTANTIATE_TEST_SUITE_P(authoritys, TestUriAuthority, testing::Values(
  std::make_tuple("0.0.0.0", "80", AF_INET),
  std::make_tuple("::", "80", AF_INET6),
  std::make_tuple("0000:0000:0000:0000:0000:0000:0000:0001", "80", AF_INET6),
  std::make_tuple("2001:0db8:ab00:0000:0000:0000:0000:0000", "9000", AF_INET6),
  std::make_tuple("2001:0db8:c9d2:0012:0000:0000:0000:0051", "9000", AF_INET6),
  std::make_tuple("2001:0db8:c9d2:0000:0001:0000:0000:0051", "9000", AF_INET6),
  std::make_tuple("1:0:0001:0000:010:0000:0100:0000", "9000", AF_INET6),
  std::make_tuple("0:0:0001:0000:000:0000:0100:0000", "9000", AF_INET6)
));

TEST_P(TestUriAuthoritySerialized, operator_equal)
{
  uriAuthoritySerializedTestCase params = GetParam();
  uri::Authority authority(std::get<0>(params), std::get<1>(params));
  uri::Authority authority2(std::get<3>(params), std::get<1>(params));

  EXPECT_EQ(authority, authority2);
}

INSTANTIATE_TEST_SUITE_P(authoritys, TestUriAuthoritySerialized, testing::Values(
  std::make_tuple("0.0.0.0", "80", AF_INET, "0.0.0.0"),
  std::make_tuple("::", "80", AF_INET6, "::"),
  std::make_tuple("0000:0000:0000:0000:0000:0000:0000:0001", "80", AF_INET6, "::1"),
  std::make_tuple("2001:0db8:ab00:0000:0000:0000:0000:0000", "9000", AF_INET6, "2001:db8:ab00::"),
  std::make_tuple("2001:0db8:c9d2:0012:0000:0000:0000:0051", "9000", AF_INET6, "2001:db8:c9d2:12::51"),
  std::make_tuple("2001:0db8:c9d2:0000:0001:0000:0000:0051", "9000", AF_INET6, "2001:db8:c9d2:0:1::51"),
  std::make_tuple("1:0:0001:0000:010:0000:0100:0000", "9000", AF_INET6, "1:0:1:0:10:0:100:0"),
  std::make_tuple("0:0:0001:0000:000:0000:0100:0000", "9000", AF_INET6, "0:0:1::100:0")
));
