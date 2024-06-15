#pragma once

#include <string>

#include "Arenas.hpp"
#include "Request.hpp"

typedef int ParseLength;
#define ParseLengthInvalid -1

enum  ParseError
{
  kNone,
  kSyntaxError, // status 400 (close the socket)
  kWrongMethod, // status 400
  kWrongRequestTarget, // status 400
  kWrongHeader,        // status 400
  kUnsupportedMethod, // status 501
  kUnsupportedRequestTarget, // status 501
  kUnsupportedHttpVersion, // status 505
  kUnsupportedIpAddress,   // status 505
  kUnsupportedScheme // status 505
};

StatusCode  ParseErrorToStatusCode(enum ParseError error);

namespace http_parser
{
struct PTNodeRequestLine;
struct PTNodeFields;
}

enum ParseError AnalysisRequestLine(http_parser::PTNodeRequestLine* request_line, RequestLine* output);
enum ParseError AnalysisRequestHeaders(http_parser::PTNodeFields* fields, HeaderMap* output);

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
    const char*   bytes;
    unsigned int  length;

    StringSlice();
    StringSlice(const char* bytes, unsigned int length);
    std::string to_string() const;
    bool  is_valid() const;
    const char* consume(unsigned int amount = 1);
    int   match(const char* string);
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
    kFields,

    kFieldContentType,
    kFieldContentLength,
    kFieldConnection,
    kFieldHost,
    kFieldTransferEncoding,
    kFieldReferer,
    kFieldUserAgent
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

  struct ParseOutput
  {
    PTNode*           result;
    int               length;

    ParseOutput();
    bool  is_valid() const;
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
    StringSlice content;
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
    PTNodeUriPort*      port;
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
    enum Method method; 
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

  ParseOutput  ParseReasonPhrase(Input input);
  ParseOutput  ParseStatusCode(Input input);
  ParseOutput  ParseStatusLine(Input input);

  ////////////////////////////////////////////
  ////////////////   fields   ////////////////
  ////////////////////////////////////////////

  struct PTNodeFieldValue : public PTNode
  {
    StringSlice content;
  };

  struct PTNodeFieldLine : public PTNode
  {
    PTNodeToken*       name;
    PTNodeFieldValue*  value;
  };

  struct PTNodeFields : public PTNode
  {
    temporary::vector<PTNodeFieldLine*> fields;
  };

  ParseOutput  ParseFieldValue(Input input);
  ParseOutput  ParseFieldLine(Input input);
  ParseOutput  ParseFields(Input input);

  //////////////////////////////////////////////////////////
  ////////////////   specific field value   ////////////////
  //////////////////////////////////////////////////////////

  struct PTNodeFieldContentType : public PTNode
  {
    StringSlice       content;
    PTNodeParameters* parameters;
  };

  struct PTNodeFieldContentLength : public PTNode
  {
    int number;
  };

  struct PTNodeFieldConnection : public PTNode
  {
    temporary::vector<PTNodeToken*> options;
    // Only useful option is "close"
  };

  struct PTNodeFieldHost : public PTNode
  {
    PTNodeUriHost*  host;
    PTNodeUriPort*  port;
  };

  struct PTNodeFieldReferer : public PTNode
  {
    union
    {
      PTNode*                         uri_header;
      PTNodeUriAbsolute*              uri_absolute;
      PTNodeUriReferenceNetworkPath*  network_path_reference;
      PTNodePathAbsolute*             path_absolute;
      PTNodePathNoScheme*             path_noscheme;
      PTNodePathEmpty*                path_empty;
    };
  };

  struct PTNodeFieldTransferEncoding : public PTNode
  {
    temporary::vector<StringSlice> codings;
  };

  /*Content-Type = media-type

  media-type = type "/" subtype parameters

  type = token

  subtype = token*/
  ParseOutput ParseFieldContentType(Input input);

  // Content-Length      = 1*DIGIT
  ParseOutput ParseFieldContentLength(Input input);

  /*Connection = [ connection-option *( OWS "," OWS connection-option ) ]

  connection-option = token*/
  ParseOutput ParseFieldConnection(Input input);

  /*RFC9110 7.2

  Host = uri-host [ ":" port ] */
  ParseOutput ParseFieldHost(Input input);

  /* TE = [ t-codings *( OWS "," OWS t-codings ) ]
  t-codings = "trailers" / ( transfer-coding [ weight ] )*/
  ParseOutput ParseFieldTransferEncoding(Input input);

  /* Referer = absolute-URI / partial-URI

  partial-URI = relative-part [ "?" query ] */
  ParseOutput ParseFieldReferer(Input input);

  /////////////////////////////////////////////////////
  ///  fields used only in response (unimplemented) ///
  /////////////////////////////////////////////////////

  // Last-Modified = HTTP-date
  ParseOutput ParseFieldLastModified(Input input);

  // Allow = [ method *( OWS "," OWS method ) ]
  ParseOutput ParseFieldAllow(Input input);

  // Location = URI-reference
  ParseOutput ParseFieldLocation(Input input);

  /* Server              = product *( RWS ( product / comment ) )
  product = token [ "/" product-version ] */
  ParseOutput ParseFieldServer(Input input);
} // namespace http_parser

