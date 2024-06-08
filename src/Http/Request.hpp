#pragma once

#include <string>
#include <map>

#include "Uri/Uri.hpp"
#include "Protocol.hpp"
#include "Http/HeaderValue/HeaderValue.hpp"

struct RequestLine
{
  Method  method;
  Uri     requestTarget;
  Version version;

  RequestLine();
};

typedef std::map<std::string, HeaderValue*> HeaderMap;

struct Request
{
  RequestLine   request_line;
  HeaderMap     headers;
  std::string   body;

  ~Request();
  void  clear_headers();
};

