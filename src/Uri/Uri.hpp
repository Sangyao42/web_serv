#pragma once

#include <string>
#include <vector>
#include <utility>

#include "Uri/Authority.hpp"

namespace uri
{
  typedef std::vector<std::pair<std::string, std::string> > Query;
} // namespace uri

struct Uri
{
  std::string     scheme;
  uri::Authority  authority;
  std::string     path;
  uri::Query      query;
  std::string     fragment;
};
