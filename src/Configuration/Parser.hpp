#pragma once

#include "Http/Parser.hpp"

namespace directive_parser
{
  typedef http_parser::ParseOutput  ParseOutput;
  typedef http_parser::Input        ParseInput;
  typedef http_parser::ScanOutput   ScanOutput;

  ParseOutput ParseMainBlock(ParseInput input);
  ParseOutput ParseEventsBlock(ParseInput input);
  ParseOutput ParseHttpBlock(ParseInput input);
  ParseOutput ParseServerBlock(ParseInput input);
  ParseOutput ParseLocationBlock(ParseInput input);

  ParseOutput ParseRoot(ParseInput input); // absolute path
  ParseOutput ParseIndex(ParseInput input);
  ParseOutput ParseAutoIndex(ParseInput input);
  ParseOutput ParseClientMaxBodySize(ParseInput input);
  ParseOutput ParseAccessLog(ParseInput input);
  ParseOutput ParseErrorLog(ParseInput input);
  ParseOutput ParseWorkerConnections(ParseInput input);

  ParseOutput ParseAllowMethods(ParseInput input);
  ParseOutput ParseCgi(ParseInput input);
  ParseOutput ParseErrorPages(ParseInput input);
  ParseOutput ParseListen(ParseInput input);
  ParseOutput ParseMimeTypes(ParseInput input);
  ParseOutput ParseReturn(ParseInput input);
  ParseOutput ParseServerName(ParseInput input);
} // namespace directive

