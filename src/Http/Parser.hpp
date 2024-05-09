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

    kHttpVersion,

    kRequestLine,
    kRequestTargetOriginForm,
    kRequestTargetAbsoluteForm,
    kRequestTargetAuthorityForm,
    kRequestAsteriskForm,

    kStatusLine,
    kStatusCode,
    kStatusReasonPhrase,

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
    ParseNodeType   value_type;
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

  struct ParseNodeHttpVersion : public ParseNode
  {
    std::string content;
  };

  //////////////////////////////////////////////////
  ////////////////   Request line   ////////////////
  //////////////////////////////////////////////////

  struct ParseNodeRequestTargetOriginForm : public ParseNode 
  {
    // TODO: need to parse URI stuffs
  };

  struct  ParseNodeRequestTargetAbsoluteForm : public ParseNode
  {
    // TODO: need to parse URI stuffs
  };

  struct ParseNodeRequestTargetAuthorityForm : public ParseNode
  {
    // TODO: need to parse URI stuffs
  };

  struct ParseNodeRequestTargetAsteriskForm : public ParseNode
  {};
  
  struct ParseNodeRequestLine : public ParseNode
  {
    ParseNodeToken* method;
    ParseNodeType   request_target_type;
    union
    {
      ParseNodeRequestTargetOriginForm*     request_target_origin_form;
      ParseNodeRequestTargetAbsoluteForm*   request_target_absolute_form;
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

  struct ParseNodeStatusLine : public ParseNode
  {
    ParseNodeHttpVersion*         http_version;
    int                           status_code;
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

