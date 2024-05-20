#pragma once

#include <string>

#include "Arenas.hpp"

namespace http_parser
{
  ///////////////////////////////////////////////////////////
  ////////////////   scan and parse units    ////////////////
  ///////////////////////////////////////////////////////////

  bool  IsAlpha(char character);
  bool  IsBit(char character);
  bool  IsChar(char character);
  bool  IsCarriageReturn(char character);
  bool  IsControls(char character);
  bool  IsDigit(char character);
  bool  IsDoubleQuote(char character);
  bool  IsHexDigit(char character);
  bool  IsHorizontalTab(char character);
  bool  IsLinefeed(char character);
  bool  IsSpace(char character);
  bool  IsVisibleCharacter(char character);
  bool  IsWhitespace(char character);

  bool  IsTokenText(char character);
  bool  IsQuotedStringText(char character);
  bool  IsCommentText(char character);
  bool  IsOpaqueText(unsigned char character); // named as obs-text in abnf
  bool  IsEscapedText(char character); // named as quoted-pair in abnf

  //////////////////////////////////////////
  ////////////////   scan   ////////////////
  //////////////////////////////////////////

  struct StringSlice
  {
    const char* bytes;
    int         length;

    StringSlice();
    StringSlice(const char* bytes, int length);
    std::string to_string() const;
    bool  is_valid() const;
    const char* consume(int amount = 1);
  };

  typedef StringSlice Input;
  typedef StringSlice ScanOutput;

  ScanOutput  ScanNewLine(Input input);
  ScanOutput  ScanLinearWhiteSpace(Input input);
  ScanOutput  ScanOptionalWhitespace(Input input);
  ScanOutput  ScanRequiredWhitespace(Input input);
  ScanOutput  ScanBadWhitespace(Input input);

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
    kMethod,

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

  enum ParseOutputStatus
  {
    kParseFailure,
    kParseSuccess
  };

  struct ParseOutput
  {
    ParseOutputStatus status;
    int               parsed_length;
    PTNode*           result;

    ParseOutput();
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
    temporary::vector<PTNodeParameter*> children;
  };

  ParseOutput  ParseToken(Input input);
  ParseOutput  ParseQuotedString(Input input);
  ParseOutput  ParseComment(Input input);
  ParseOutput  ParseParameter(Input input);
  ParseOutput  ParseParameters(Input input);

  /////////////////////////////////////////
  ////////////////   path   ///////////////
  /////////////////////////////////////////

  bool  IsUnreservered(char character);
  bool  IsReservered(char character);
  bool  IsGenDelims(char character);
  bool  IsSubDelims(char character);

  ScanOutput  ScanPathChar(Input input);
  ScanOutput  ScanPctEncoded(Input input);
  ScanOutput  ScanSegment(Input input);
  ScanOutput  ScanSegments(Input input);
  ScanOutput  ScanSegmentNz(Input input);
  ScanOutput  ScanSegmentNzNc(Input input);

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

  ScanOutput   ScanDecOctet(Input input);
  ScanOutput   ScanH16(Input input);

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
    PTNodeUriUserInfo*  user_info;
    PTNodeUriHost*      host;
    PTNodeUriQuery*     query;
  };

  struct PTNodeUriFragment : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeUriReferenceNetworkPath : public PTNode
  {
    PTNodeUriAuthority* authority;
    PTNodePathAbEmpty*  path;
  };

  struct PTNodeUri : public PTNode
  {
    PTNodeUriScheme*  scheme;
    union
    {
      PTNode*                        path_header;
      PTNodeUriReferenceNetworkPath* network_path_reference;
      PTNodePathAbsolute*            path_absolute;
      PTNodePathRootless*            path_rootless;
      PTNodePathEmpty*               path_empty;
    };
    PTNodeUriQuery*    query;
    PTNodeUriFragment* fragment;
  };

  struct PTNodeUriAbsolute : public PTNode
  {
    PTNodeUriScheme*  scheme;
    union
    {
      PTNode*                        path_header;
      PTNodeUriReferenceNetworkPath* network_path_reference;
      PTNodePathAbsolute*            path_absolute;
      PTNodePathRootless*            path_rootless;
      PTNodePathEmpty*               path_empty;
    };
    PTNodeUriQuery*   query;
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
    PTNodeUriQuery*    query;
    PTNodeUriFragment* fragment;
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

  ScanOutput  ScanStartLineSpaces(Input input);

  struct PTNodeHttpVersion : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeMethod : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeRequestTargetOriginForm : public PTNode 
  {
    PTNodePathAbsolute* absolute_path;
    PTNodeUriQuery*     query;
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
    PTNodeMethod*        method;
    union
    {
      PTNode*                            request_target_header;
      PTNodeRequestTargetOriginForm*     request_target_origin_form;
      PTNodeUriAbsolute*                 request_target_absolute_form; // only used in CONNECT message
      PTNodeRequestTargetAuthorityForm*  request_target_authority_form;
      PTNodeRequestTargetAsteriskForm*   request_target_asterisk_form;
    };
    PTNodeHttpVersion*  version;
  };

  ParseOutput  ParseHttpVersion(Input input);
  ParseOutput  ParseMethod(Input input);
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
    PTNodeHttpVersion*  http_version;
    PTNodeStatusCode*   status_code;
    PTNodeReasonPhrase* reason_phrase;
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
    temporary::vector<PTNodeFieldLine*> fields;
    PTNodeMessageBody*                  body;
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

