#pragma once

#include <string>
#include <vector>
#include <utility>

#include "misc/Maybe.hpp"

namespace uri
{
  struct Host
  {
    enum Type
    {
      IPV4,
      IPV6,
      REGNAME
    }           type;
    std::string value;
  };

  struct Authority
  {
    Maybe<std::string>  userinfo;
    std::string         host;
    Maybe<int>          port;
  };

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
