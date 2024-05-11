#pragma once

#include <vector>
#include <string>

#include "misc/Maybe.hpp"
#include "Arenas.hpp"

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

    std::string to_string() const;
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

  enum ParseOutputError
  {
    kParseSuccess
  };

  struct ParseOutput
  {
    ParseOutputError  error;
    Input             rest;
  };

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

  struct ParseTree
  {
    PTNode*  root;
    PTNode*  current;
  };

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
    PTNodeType   type_value;
    union
    {
      PTNodeToken*         value;
      PTNodeQuotedString*  value_quoted;
    };
  };

  struct PTNodeParameters : public PTNode
  {
    std::vector<PTNodeParameter*> children;
  };

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

  /////////////////////////////////////////
  ////////////////   uri   ////////////////
  /////////////////////////////////////////

  struct PTNodeUriScheme : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeUriHost : public PTNode
  {
    PTNodeType type_address;
    union
    {
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
    PTNodeType                type_path;
    union
    {
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
    PTNodeType                type_path;
    union
    {
      PTNodeUriReferenceNetworkPath* network_path_reference;
      PTNodePathAbsolute*            path_absolute;
      PTNodePathRootless*            path_rootless;
      PTNodePathEmpty*               path_empty;
    };
    Maybe<PTNodeUriQuery*>    query;
  };

  struct PTNodeUriReferenceRelative : public PTNode
  {
    PTNodeType type_path;
    union
    {
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
    PTNodeType type_reference;
    union
    {
      PTNodeUri*                   uri;
      PTNodeUriReferenceRelative*  relative_reference;
    };
  };

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
    PTNodeType   type_request_target;
    union
    {
      PTNodeRequestTargetOriginForm*     request_target_origin_form;
      PTNodeUriAbsolute*                 request_target_absolute_form; // only used in CONNECT message
      PTNodeRequestTargetAuthorityForm*  request_target_authority_form;
      PTNodeRequestTargetAsteriskForm*   request_target_asterisk_form;
    };
    PTNodeHttpVersion* version;
  };

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
    PTNodeType start_line_type;
    union
    {
      PTNodeRequestLine* request_line;
      PTNodeStatusLine*  status_line;
    };
    std::vector<PTNodeFieldLine*>  fields;
    Maybe<PTNodeMessageBody*>      body;
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

