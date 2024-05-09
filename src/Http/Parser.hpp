#pragma once

#include <string>
#include <vector>

namespace http_parser
{

  struct StringSlice
  {
    const char* bytes;
    int         length;
  };

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

  /* token                = 1*tchar
  tchar                =
   "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /
   "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA

  ; Quoted Strings (RFC9110 5.6.4)
  quoted-string        = DQUOTE *( qdtext / quoted-pair ) DQUOTE
  qdtext               =
   HTAB / SP / "!" / %x23-5B ; '#'-'['
   / %x5D-7E ; ']'-'~'
   / obs-text
  quoted-pair          = "\" ( HTAB / SP / VCHAR / obs-text )

  ; comment (RFC9110 5.6.5)
  comment              = "(" *( ctext / quoted-pair / comment ) ")"
  ctext                =
   HTAB / SP / %x21-27 ; '!'-'''
   / %x2A-5B ; '*'-'['
   / %x5D-7E ; ']'-'~'
   / obs-text
  
  ; Parameters a.k.a key and value pairs delimited by '=' and delimited by ';' (RFC9110 5.6.6) 
  parameters           = *( OWS ";" OWS [ parameter ] )
  parameter            = parameter-name "=" parameter-value
  parameter-name       = token
  parameter-value      = ( token / quoted-string ) */

  enum TokenType
  {
    kWord,
    kQuotedWord,
    kDquote,
    kLBracket,
    kRBracket

  };

  struct Token
  {
    TokenType   type;
    StringSlice string;
  };

  class Tokenizer
  {
    public:
      Tokenizer();
      Tokenizer(const std::string& string);

      void  set_string(const std::string& string);
      Token tokenize();
      void  untokenize(Token token);

    private:
      const char*         start;
      const char*         end;
      const char*         current;
      std::vector<Token>  untokenized; // used as a stack of tokens
  };

  typedef const char* Input;
  struct Output
  {
    int     length;
    Input   rest;

    bool    is_success();
  };

  Output  ParseAlpha(Input input);
  Output  ParseBit(Input input);
  Output  ParseChar(Input input);
  Output  ParseCarriageReturn(Input input);
  Output  ParseControls(Input input);
  Output  ParseDigit(Input input);
  Output  ParseDoubleQuote(Input input);
  Output  ParseHexDigit(Input input);
  Output  ParseHorizontalTab(Input input);
  Output  ParseLinefeed(Input input);
  Output  ParseLinearWhiteSpace(Input input);
  Output  ParseOctet(Input input);
  Output  ParseSpace(Input input);
  Output  ParseVisibleCharacter(Input input);
  Output  ParseWhitespace(Input input);

  Output  ParseOptionalWhitespace(Input input);
  Output  ParseRequiredWhitespace(Input input);
  Output  ParseBadWhitespace(Input input);

  ///////////////////////////////////////////
  ////////////////   text    ////////////////
  ///////////////////////////////////////////

  Output  ParseQuotedStringText(Input input);
  Output  ParseCommentText(Input input);
  Output  ParseOpaqueText(Input input); // named as obs-text in abnf
  Output  ParseEscapedText(Input input); // named as quoted-pair in abnf

  ///////////////////////////////////////////
  ////////////////   token   ////////////////
  ///////////////////////////////////////////

  typedef Output  AToken;

  AToken   ParseToken(Input input);
  Output  ParseTokenCharacter(Input input);

  Output  ParseQuotedString(Input input);
  Output  ParseComment(Input input);

  Output  ParseHttpMessage(Input input); 
} // namespace http_parser

