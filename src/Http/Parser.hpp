#pragma once

#include <vector>
#include <string>

#include "misc/Maybe.hpp"
#include "Arenas.hpp"

namespace http_parser
{
  struct StringSlice
  {
    const char* bytes;
    int         length;

    std::string to_string() const;
  };

  typedef const char* Input;

  //////////////////////////////////////////
  ////////////////   scan   ////////////////
  //////////////////////////////////////////

  struct ScanOutput
  {
    bool        is_success;
    StringSlice string;
    Input       rest;
  };

  ScanOutput  ScanAlpha(Input input);
  ScanOutput  ScanBit(Input input);
  ScanOutput  ScanChar(Input input);
  ScanOutput  ScanCarriageReturn(Input input);
  ScanOutput  ScanControls(Input input);
  ScanOutput  ScanDigit(Input input);
  ScanOutput  ScanDoubleQuote(Input input);
  ScanOutput  ScanHexDigit(Input input);
  ScanOutput  ScanHorizontalTab(Input input);
  ScanOutput  ScanLinefeed(Input input);
  ScanOutput  ScanLinearWhiteSpace(Input input);
  ScanOutput  ScanOctet(Input input);
  ScanOutput  ScanSpace(Input input);
  ScanOutput  ScanVisibleCharacter(Input input);
  ScanOutput  ScanWhitespace(Input input);

  ScanOutput  ScanOptionalWhitespace(Input input);
  ScanOutput  ScanRequiredWhitespace(Input input);
  ScanOutput  ScanBadWhitespace(Input input);

  ///////////////////////////////////////////
  ////////////////   text    ////////////////
  ///////////////////////////////////////////

  ScanOutput  ScanQuotedStringText(Input input);
  ScanOutput  ScanCommentText(Input input);
  ScanOutput  ScanOpaqueText(Input input); // named as obs-text in abnf
  ScanOutput  ScanEscapedText(Input input); // named as quoted-pair in abnf

  ///////////////////////////////////////////
  ////////////////   parse   ////////////////
  ///////////////////////////////////////////

  enum PTNodeType
  {
    kToken,
    kQuotedString,
    kComment,
    kParameter,
    kParameters,

    kPathAbempty,
    kPathAbsolute,
    kPathNoScheme,
    kPathRootless,
    kPathEmpty,

    kIpv6Address,
    kIpvFutureAddress,
    kIpv4Address,
    kRegName,

    kUriScheme,
    kUriHost,
    kUriPort,
    kUriQuery,
    kUriUserInfo,
    kUriAuthority,
    kUriFragment,
    kUriReferenceNetworkPath,
    kUri,
    kUriAbsolute,
    kUriReferenceRelative,
    kUriReference,

    kHttpVersion,

    kRequestLine,
    kRequestTargetOriginForm,
    kRequestTargetAuthorityForm,
    kRequestAsteriskForm,

    kStatusReasonPhrase,
    kStatusLine,
    kStatusCode,

    kFieldValue,
    kFieldLine,
    kMessageBody,
    kMessage
  };

  struct PTNode
  {
    PTNodeType type;
  };

  template <typename T>
  T*  PTNodeCreate()
  {
    return temporary::arena.allocate<T>(); 
  }

  enum ParseOutputError
  {
    kParseSuccess
  };

  struct ParseOutput
  {
    ParseOutputError  error;
    Input             rest;
    PTNode*           result;
  };

  ///////////////////////////////////////////
  ////////////////   token   ////////////////
  ///////////////////////////////////////////

  struct PTNodeToken : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeQuotedString : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeComment : public PTNode
  {}; // ignore comment content

  struct PTNodeParameter : public PTNode
  {
    PTNodeToken* name;
    union
    {
      PTNode*              value_header;
      PTNodeToken*         value;
      PTNodeQuotedString*  value_quoted;
    };
  };

  struct PTNodeParameters : public PTNode
  {
    std::vector<PTNodeParameter*> children;
  };

  ParseOutput  ParseToken(Input input);
  ParseOutput  ParseQuotedString(Input input);
  ParseOutput  ParseComment(Input input);
  ParseOutput  ParseParameter(Input input);
  ParseOutput  ParseParameters(Input input);

  /////////////////////////////////////////
  ////////////////   path   ///////////////
  /////////////////////////////////////////

  struct PTNodePathAbEmpty : public PTNode
  {
    StringSlice content;
  };

  struct PTNodePathAbsolute : public PTNode
  {
    StringSlice content;
  };

  struct PTNodePathNoScheme : public PTNode
  {
    StringSlice content;
  };

  struct PTNodePathRootless : public PTNode
  {
    StringSlice content;
  };

  struct PTNodePathEmpty : public PTNode
  {};

  ParseOutput  ParsePathAbEmpty(Input input);
  ParseOutput  ParsePathAbsolute(Input input);
  ParseOutput  ParsePathNoScheme(Input input);
  ParseOutput  ParsePathRootless(Input input);
  ParseOutput  ParsePathEmpty(Input input);

  ////////////////////////////////////////////////
  ////////////////   ip address   ////////////////
  ////////////////////////////////////////////////

  struct PTNodeIpv6Address : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeIpvFutureAddress : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeIpv4Address : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeRegName : public PTNode
  {
    StringSlice content;
  };

  ParseOutput  ParseIpv6Address(Input input);
  ParseOutput  ParseIpvFutureAddress(Input input);
  ParseOutput  ParseIpv4Address(Input input);
  ParseOutput  ParseRegName(Input input);

  /////////////////////////////////////////
  ////////////////   uri   ////////////////
  /////////////////////////////////////////

  struct PTNodeUriScheme : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeUriHost : public PTNode
  {
    union
    {
      PTNode*                  address_header;
      PTNodeIpv6Address*       ipv6_address;
      PTNodeIpvFutureAddress*  ipvfuture_address;
      PTNodeIpv4Address*       ipv4_address;
      PTNodeRegName*           reg_name;
    };
  };

  struct PTNodeUriPort : public PTNode
  {
    int number;
  };

  struct PTNodeUriQuery : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeUriUserInfo : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeUriAuthority : public PTNode
  {
    Maybe<PTNodeUriUserInfo*>  user_info;
    PTNodeUriHost*             host;
    Maybe<PTNodeUriQuery*>     query;
  };

  struct PTNodeUriFragment : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeUriReferenceNetworkPath : public PTNode
  {
    PTNodeUriAuthority*   authority;
    PTNodePathAbEmpty* path;
  };

  struct PTNodeUri : public PTNode
  {
    PTNodeUriScheme*          scheme;
    union
    {
      PTNode*                        path_header;
      PTNodeUriReferenceNetworkPath* network_path_reference;
      PTNodePathAbsolute*            path_absolute;
      PTNodePathRootless*            path_rootless;
      PTNodePathEmpty*               path_empty;
    };
    Maybe<PTNodeUriQuery*>    query;
    Maybe<PTNodeUriFragment*> fragment;
  };

  struct PTNodeUriAbsolute : public PTNode
  {
    PTNodeUriScheme*          scheme;
    union
    {
      PTNode*                        path_header;
      PTNodeUriReferenceNetworkPath* network_path_reference;
      PTNodePathAbsolute*            path_absolute;
      PTNodePathRootless*            path_rootless;
      PTNodePathEmpty*               path_empty;
    };
    Maybe<PTNodeUriQuery*>    query;
  };

  struct PTNodeUriReferenceRelative : public PTNode
  {
    union
    {
      PTNode*                        path_header;
      PTNodeUriReferenceNetworkPath* network_path_reference;
      PTNodePathAbsolute*            path_absolute;
      PTNodePathNoScheme*            path_noscheme;
      PTNodePathEmpty*               path_empty;
    };
    Maybe<PTNodeUriQuery*>    query;
    Maybe<PTNodeUriFragment*> fragment;
  };

  struct PTNodeUriReference : public PTNode
  {
    union
    {
      PTNode*                      uri_header;
      PTNodeUri*                   uri;
      PTNodeUriReferenceRelative*  relative_reference;
    };
  };

  ParseOutput  ParseUriScheme(Input input);
  ParseOutput  ParseUriHost(Input input);
  ParseOutput  ParseUriPort(Input input);
  ParseOutput  ParseUriQuery(Input input);
  ParseOutput  ParseUriUserInfo(Input input);
  ParseOutput  ParseUriAuthority(Input input);
  ParseOutput  ParseUriFragment(Input input);
  ParseOutput  ParseUriReferenceNetworkPath(Input input);
  ParseOutput  ParseUri(Input input);
  ParseOutput  ParseUriAbsolute(Input input);
  ParseOutput  ParseUriReferenceRelative(Input input);
  ParseOutput  ParseUriReference(Input input);

  //////////////////////////////////////////////////
  ////////////////   Request line   ////////////////
  //////////////////////////////////////////////////

  struct PTNodeHttpVersion : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeRequestTargetOriginForm : public PTNode 
  {
    PTNodePathAbsolute*    absolute_path;
    Maybe<PTNodeUriQuery*> query;
  };

  struct PTNodeRequestTargetAuthorityForm : public PTNode
  {
    PTNodeUriHost* host;
    PTNodeUriPort* port;
  };

  struct PTNodeRequestTargetAsteriskForm : public PTNode
  {};

  struct PTNodeRequestLine : public PTNode
  {
    PTNodeToken* method;
    union
    {
      PTNode*                            request_target_header;
      PTNodeRequestTargetOriginForm*     request_target_origin_form;
      PTNodeUriAbsolute*                 request_target_absolute_form; // only used in CONNECT message
      PTNodeRequestTargetAuthorityForm*  request_target_authority_form;
      PTNodeRequestTargetAsteriskForm*   request_target_asterisk_form;
    };
    PTNodeHttpVersion* version;
  };

  ParseOutput  ParseHttpVersion(Input input);
  ParseOutput  ParseRequestTargetOriginForm(Input input);
  ParseOutput  ParseRequestTargetAuthorityForm(Input input);
  ParseOutput  ParseRequestTargetAsteriskForm(Input input);
  ParseOutput  ParseRequestLine(Input input);

  /////////////////////////////////////////////////
  ////////////////   status line   ////////////////
  /////////////////////////////////////////////////

  struct PTNodeReasonPhrase : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeStatusCode : public PTNode
  {
    int number;
  };

  struct PTNodeStatusLine : public PTNode
  {
    PTNodeHttpVersion*         http_version;
    PTNodeStatusCode*          status_code;
    Maybe<PTNodeReasonPhrase*> reason_phrase;
  };

  struct PTNodeFieldValue : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeFieldLine : public PTNode
  {
    PTNodeToken*       name;
    PTNodeFieldValue*  value;
  };

  struct PTNodeMessageBody : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeMessage : public PTNode
  {
    union
    {
      PTNode*            start_line_header;
      PTNodeRequestLine* request_line;
      PTNodeStatusLine*  status_line;
    };
    std::vector<PTNodeFieldLine*>  fields;
    Maybe<PTNodeMessageBody*>      body;
  };

  ParseOutput  ParseReasonPhrase(Input input);
  ParseOutput  ParseStatusCode(Input input);
  ParseOutput  ParseStatusLine(Input input);
  ParseOutput  ParseFieldValue(Input input);
  ParseOutput  ParseFieldLine(Input input);
  ParseOutput  ParseMessageBody(Input input);
  ParseOutput  ParseRequestMessage(Input input);
  ParseOutput  ParseResponseMessage(Input input);

} // namespace http_parser

