#pragma once

#include <vector>
#include <string>

#include "misc/Maybe.hpp"

namespace http_parser
{
  /* enum CharType
  {
    kAlpha,
    kCr,
    kCtl,
    kDigit,
    kDquote,
    kHtab,
    kLf,
    kSp,
  }; */

  struct StringSlice
  {
    const char* bytes;
    int         length;
  };

  typedef const char* Input;
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
  ////////////////   token   ////////////////
  ///////////////////////////////////////////

  enum ParseNodeType
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

  struct ParseNode
  {
    ParseNodeType type;
  };

  struct ParseNodeToken : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeQuotedString : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeComment : public ParseNode
  {}; // ignore comment content

  struct ParseNodeParameter : public ParseNode
  {
    ParseNodeToken* name;
    ParseNodeType   type_value;
    union
    {
      ParseNodeToken*         value;
      ParseNodeQuotedString*  value_quoted;
    };
  };

  struct ParseNodeParameters : public ParseNode
  {
    std::vector<ParseNodeParameter*> children;
  };

  /////////////////////////////////////////
  ////////////////   path   ///////////////
  /////////////////////////////////////////

  struct ParseNodePathAbEmpty : public ParseNode
  {
    std::string content;
  };

  struct ParseNodePathAbsolute : public ParseNode
  {
    std::string content;
  };

  struct ParseNodePathNoScheme : public ParseNode
  {
    std::string content;
  };

  struct ParseNodePathRootless : public ParseNode
  {
    std::string content;
  };

  struct ParseNodePathEmpty : public ParseNode
  {};

  ////////////////////////////////////////////////
  ////////////////   ip address   ////////////////
  ////////////////////////////////////////////////

  struct ParseNodeIpv6Address : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeIpvFutureAddress : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeIpv4Address : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeRegName : public ParseNode
  {
    std::string content;
  };

  /////////////////////////////////////////
  ////////////////   uri   ////////////////
  /////////////////////////////////////////

  struct ParseNodeUriScheme : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeUriHost : public ParseNode
  {
    ParseNodeType type_address;
    union
    {
      ParseNodeIpv6Address*       ipv6_address;
      ParseNodeIpvFutureAddress*  ipvfuture_address;
      ParseNodeIpv4Address*       ipv4_address;
      ParseNodeRegName*           reg_name;
    };
  };

  struct ParseNodeUriPort : public ParseNode
  {
    int number;
  };

  struct ParseNodeUriQuery : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeUriUserInfo : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeUriAuthority : public ParseNode
  {
    Maybe<ParseNodeUriUserInfo*>  user_info;
    ParseNodeUriHost*             host;
    Maybe<ParseNodeUriQuery*>     query;
  };

  struct ParseNodeUriFragment : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeUriReferenceNetworkPath : public ParseNode
  {
    ParseNodeUriAuthority*   authority;
    ParseNodePathAbEmpty* path;
  };

  struct ParseNodeUri : public ParseNode
  {
    ParseNodeUriScheme*          scheme;
    ParseNodeType                type_path;
    union
    {
      ParseNodeUriReferenceNetworkPath* network_path_reference;
      ParseNodePathAbsolute*            path_absolute;
      ParseNodePathRootless*            path_rootless;
      ParseNodePathEmpty*               path_empty;
    };
    Maybe<ParseNodeUriQuery*>    query;
    Maybe<ParseNodeUriFragment*> fragment;
  };

  struct ParseNodeUriAbsolute : public ParseNode
  {
    ParseNodeUriScheme*          scheme;
    ParseNodeType                type_path;
    union
    {
      ParseNodeUriReferenceNetworkPath* network_path_reference;
      ParseNodePathAbsolute*            path_absolute;
      ParseNodePathRootless*            path_rootless;
      ParseNodePathEmpty*               path_empty;
    };
    Maybe<ParseNodeUriQuery*>    query;
  };

  struct ParseNodeUriReferenceRelative : public ParseNode
  {
    ParseNodeType type_path;
    union
    {
      ParseNodeUriReferenceNetworkPath* network_path_reference;
      ParseNodePathAbsolute*            path_absolute;
      ParseNodePathNoScheme*            path_noscheme;
      ParseNodePathEmpty*               path_empty;
    };
    Maybe<ParseNodeUriQuery*>    query;
    Maybe<ParseNodeUriFragment*> fragment;
  };

  struct ParseNodeUriReference : public ParseNode
  {
    ParseNodeType type_reference;
    union
    {
      ParseNodeUri*                   uri;
      ParseNodeUriReferenceRelative*  relative_reference;
    };
  };

  //////////////////////////////////////////////////
  ////////////////   Request line   ////////////////
  //////////////////////////////////////////////////

  struct ParseNodeHttpVersion : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeRequestTargetOriginForm : public ParseNode 
  {
    ParseNodePathAbsolute*    absolute_path;
    Maybe<ParseNodeUriQuery*> query;
  };

  struct ParseNodeRequestTargetAuthorityForm : public ParseNode
  {
    ParseNodeUriHost* host;
    ParseNodeUriPort* port;
  };

  struct ParseNodeRequestTargetAsteriskForm : public ParseNode
  {};

  struct ParseNodeRequestLine : public ParseNode
  {
    ParseNodeToken* method;
    ParseNodeType   type_request_target;
    union
    {
      ParseNodeRequestTargetOriginForm*     request_target_origin_form;
      ParseNodeUriAbsolute*                 request_target_absolute_form; // only used in CONNECT message
      ParseNodeRequestTargetAuthorityForm*  request_target_authority_form;
      ParseNodeRequestTargetAsteriskForm*   request_target_asterisk_form;
    };
    ParseNodeHttpVersion* version;
  };

  /////////////////////////////////////////////////
  ////////////////   status line   ////////////////
  /////////////////////////////////////////////////

  struct ParseNodeReasonPhrase : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeStatusCode : public ParseNode
  {
    int number;
  };

  struct ParseNodeStatusLine : public ParseNode
  {
    ParseNodeHttpVersion*         http_version;
    ParseNodeStatusCode*          status_code;
    Maybe<ParseNodeReasonPhrase*> reason_phrase;
  };

  struct ParseNodeFieldValue : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeFieldLine : public ParseNode
  {
    ParseNodeToken*       name;
    ParseNodeFieldValue*  value;
  };

  struct ParseNodeMessageBody : public ParseNode
  {
    std::string content;
  };

  struct ParseNodeMessage : public ParseNode
  {
    ParseNodeType start_line_type;
    union
    {
      ParseNodeRequestLine* request_line;
      ParseNodeStatusLine*  status_line;
    };
    std::vector<ParseNodeFieldLine*>  fields;
    Maybe<ParseNodeMessageBody*>      body;
  };

  struct ParseTree
  {
    ParseNode*  root;
    ParseNode*  current;
  };

  ScanOutput  ParseToken(Input input);
  ScanOutput  ScanTokenCharacter(Input input);

  ScanOutput  ParseQuotedString(Input input);
  ScanOutput  ScanQuotedStringCharacter(Input input);

  ScanOutput  ParseComment(Input input);
  ScanOutput  ScanCommentCharacter(Input input);

  ScanOutput  ParseParameters(Input input);
  ScanOutput  ParseParameter(Input input);

  ScanOutput  ParseHttpMessage(Input input, ParseTree* tree);
} // namespace http_parser

