#pragma once

#include <string>
#include <map>
#include "Protocol.hpp"
#include "Http/HeaderValue/HeaderValue.hpp"

struct RequestLine
{
  Method      method;
  std::string	requestTarget;
  Version     version;

  RequestLine();
};

typedef std::map<std::string, HeaderValue*> HeaderMap;

struct Request
{
  RequestLine request_line;
  HeaderMap   headers;

  ~Request();
  void  clear_headers();
};

