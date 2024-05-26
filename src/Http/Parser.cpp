#include "Parser.hpp"

#include <cstring>

namespace http_parser
{
  ///////////////////////////////////////////////////////////
  ////////////////   scan and parse units    ////////////////
  ///////////////////////////////////////////////////////////

  bool  IsAlpha(char character)
  {
    return ((character >= 'a' && character <= 'z') ||
        (character >= 'A' && character <= 'Z'));
  }

  bool  IsBit(char character)
  {
    return (character == '0' || character == '1');
  }

  bool  IsChar(char character)
  {
    return (character > 0 && character <= 127);
  }

  bool  IsCarriageReturn(char character)
  {
    return (character == '\r');
  }

  bool  IsControls(char character)
  {
    return ((character <= 31) || (character == 127));
  }

  bool  IsDigit(char character)
  {
    return (character >= '0' && character <= '9');
  }

  bool  IsDoubleQuote(char character)
  {
    return (character == '\"');
  }

  bool  IsHexDigit(char character)
  {
    return (IsDigit(character) || (character >= 'A' && character <= 'F'));
  }

  bool  IsHorizontalTab(char character)
  {
    return (character == 9);
  }

  bool  IsLinefeed(char character)
  {
    return (character == '\n');
  }

  bool  IsSpace(char character)
  {
    return (character == ' ');
  }

  bool  IsVisibleCharacter(char character)
  {
    return (character >= 21 && character <= 126);
  }

  bool  IsWhitespace(char character)
  {
    return (IsSpace(character) || IsHorizontalTab(character));
  }

  bool  IsTokenText(char character)
  {
    return ((std::strchr("!#$%&\'*+-.^_`|~", character) != NULL) ||
          IsDigit(character) ||
          IsAlpha(character));
  }

  bool  IsQuotedStringText(char character)
  {
    return (IsHorizontalTab(character) ||
            IsSpace(character) ||
            (character == '!') ||
            (character >= '#' && character <= '[') ||
            (character >= ']' && character <= '~') ||
            IsOpaqueText(character));
  }

  bool  IsCommentText(char character)
  {
    return (IsHorizontalTab(character) ||
            IsSpace(character) ||
            (character >= '!' && character <= '\'') ||
            (character >= '*' && character <= '[') ||
            (character >= ']' && character <= '~') ||
            IsOpaqueText(character));
  }

  bool  IsOpaqueText(unsigned char character)
  {
    return (character >= 128);
  }

  bool  IsEscapedText(char character)
  {
    return (IsHorizontalTab(character) ||
            IsSpace(character) ||
            IsVisibleCharacter(character) ||
            IsOpaqueText(character));
  }

  //////////////////////////////////////////
  ////////////////   scan   ////////////////
  //////////////////////////////////////////

  StringSlice::StringSlice()
    : bytes(NULL), length(0) {}

  StringSlice::StringSlice(const char* bytes, int length)
    : bytes(bytes), length(length) {}

  std::string StringSlice::to_string() const
  {
    std::string string(bytes, length);
    return string;
  }

  bool  StringSlice::is_valid() const
  {
    return (bytes != NULL);
  }

  const char* StringSlice::consume(int amount)
  {
    const char* output = NULL;
    if (amount <= length)
    {
      output = bytes;
      bytes += amount;
      length -= amount;
    }
    return output;
  }

  int  StringSlice::match(const char* string)
  {
    int  matched_length = 0;

    for (int i = 0; i < length; i++)
    {
      if (!*string || (bytes[i] != string[i]))
        break;
      else
        matched_length++;
    }
    return matched_length;
  }

  ScanOutput  ScanNewLine(Input input)
  {
    ScanOutput output;
    if (input.is_valid() &&
        (input.length >= 2) &&
        IsCarriageReturn(*input.bytes) &&
        IsLinefeed(*(input.bytes + 1)))
    {
      output.bytes = input.bytes;
      output.length = 2;
    }
    return output;
  }

  ScanOutput  ScanLinearWhiteSpace(Input input)
  {
    ScanOutput output;

    if (input.is_valid())
    {
      output.bytes = input.bytes;
      while (input.length > 0)
      {
        ScanOutput tmp = ScanNewLine(input);
        if (tmp.is_valid())
        {
          input.consume(tmp.length);
        }
        const char* target = input.consume();
        if (!IsWhitespace(*target))
          break;
        output.length += (tmp.length + 1);
      }
    }
    return output;
  }

  ScanOutput  ScanOptionalWhitespace(Input input)
  {
    ScanOutput output;

    if (input.is_valid())
    {
      output.bytes = input.bytes;
      while (input.length > 0)
      {
        const char* target = input.consume();
        if (target && !(IsSpace(*target) || IsHorizontalTab(*target)))
          break;
        output.length++;
      }
    }
    return output;
  }

  ScanOutput  ScanRequiredWhitespace(Input input)
  {
    ScanOutput output;

    if (input.is_valid())
    {
      output.bytes = input.bytes;
      while (input.length > 0)
      {
        const char* target = input.consume();
        if (target && !(IsSpace(*target) || IsHorizontalTab(*target)))
          break;
        output.length++;
      }
      if (output.length < 1)
        output.bytes = NULL;
    }
    return output;
  }

  ScanOutput  ScanBadWhitespace(Input input)
  {
    return ScanOptionalWhitespace(input);
  }

  ///////////////////////////////////////////
  ////////////////   parse   ////////////////
  ///////////////////////////////////////////

  ParseOutput::ParseOutput()
    : status(kParseFailure), parsed_length(0), result(NULL) {}

  static int ConsumeByCharacter(Input* input, char character)
  {
    int consumed_character = 0;
    if (input->length > 0)
    {
      if (*input->bytes == character)
      {
        input->consume();
        consumed_character = 1;
      }
    }
    return consumed_character;
  }

  static int ConsumeByCString(Input* input, const char* cstring)
  {
    int consumed_character = 0;
    for (int i = 0; i < input->length; i++)
    {
      if (*cstring && (*cstring == input->bytes[i]))
        consumed_character++;
    }
    if (*cstring)
      consumed_character = 0;
    else
      input->consume(consumed_character);
    return consumed_character;
  }

  static int ConsumeByUnitFunction(Input* input, bool (*func)(char))
  {
    int consumed_character = 0;
    if (input->length > 0)
    {
      if (func(*input->bytes))
      {
        input->consume();
        consumed_character = 1;
      }
    }
    return consumed_character;
  }

  static ScanOutput  ConsumeByScanFunction(Input* input, ScanOutput (*func)(Input))
  {
    ScanOutput  output = func(*input);
    if (output.is_valid())
    {
      input->consume(output.length);
    }
    return output;
  }

  static ParseOutput  ConsumeByParserFunction(Input* input, ParseOutput (*func)(Input))
  {
    ParseOutput output = func(*input);
    if (output.status == kParseSuccess)
    {
      input->consume(output.parsed_length);
    }
    return output;
  }

  ///////////////////////////////////////////
  ////////////////   token   ////////////////
  ///////////////////////////////////////////

  ParseOutput  ParseToken(Input input)
  {
    ParseOutput output;
    if (input.is_valid())
    {
      StringSlice content;
      content.bytes = input.bytes;
      while (input.length > 0)
      {
        if (!ConsumeByUnitFunction(&input, &IsTokenText))
          break;
        content.length++;
      }
      if (content.length >= 1)
      {
        PTNodeToken*  token = PTNodeCreate<PTNodeToken>();
        token->type = kToken;
        token->content = content;

        output.status = kParseSuccess;
        output.parsed_length = content.length;
        output.result = token;
      }
    }
    return output;
  }

  ParseOutput  ParseQuotedString(Input input)
  {
    ParseOutput output;
    if (input.is_valid())
    {
      StringSlice content;
      content.bytes = input.bytes;
      content.length = ConsumeByUnitFunction(&input, &IsDoubleQuote);
      if (content.length > 0)
      {
        int parsed_length = 1;
        while (input.length > 0 && parsed_length > 0)
        {
          int   parsed_length = ConsumeByUnitFunction(&input, &IsQuotedStringText);
          if (parsed_length == 0)
          {
            parsed_length = ConsumeByCharacter(&input, '\\');
            if (parsed_length > 0)
            {
              int parsed_length_2 = ConsumeByUnitFunction(&input, &IsEscapedText);
              if (parsed_length_2 > 0)
                parsed_length += parsed_length_2;
            }
          }
          content.length += parsed_length;
        }
        parsed_length = ConsumeByUnitFunction(&input, &IsDoubleQuote);
        if (parsed_length > 0)
        {
          content.length += parsed_length;
          PTNodeQuotedString*  quoted_string = PTNodeCreate<PTNodeQuotedString>();
          quoted_string->type = kQuotedString;
          quoted_string->content = content;

          output.status = kParseSuccess;
          output.parsed_length = content.length;
          output.result = quoted_string;
        }
      }
    }
    return output;
  }

  ParseOutput  ParseComment(Input input)
  {
    ParseOutput output;
    if (input.is_valid())
    {
      StringSlice content;
      content.bytes = input.bytes;
      content.length = ConsumeByCharacter(&input, '(' );
      if (content.length > 0)
      {
        int parsed_length = 1;
        while (input.length > 0 && parsed_length > 0)
        {
          int parsed_length = ConsumeByUnitFunction(&input, &IsCommentText);
          if (parsed_length == 0)
          {
            parsed_length = ConsumeByCharacter(&input, '\\');
            if (parsed_length > 0)
            {
              int parsed_length_2 = ConsumeByUnitFunction(&input, &IsEscapedText);
              if (parsed_length_2 > 0)
              {
                parsed_length += parsed_length_2;
              }
            }
          }
          if (parsed_length == 0)
          {
            ArenaSnapshot snapshot = temporary::arena.snapshot();
            ParseOutput parse_output = ConsumeByParserFunction(&input, &ParseComment);
            if (parse_output.status == kParseSuccess)
            {
              parsed_length = parse_output.parsed_length;
              temporary::arena.rollback(snapshot);
            }
          }
          content.length += parsed_length;
        }
        parsed_length = ConsumeByCharacter(&input, ')');
        if (parsed_length > 0)
        {
          content.length++;
          PTNodeComment*  comment = PTNodeCreate<PTNodeComment>();
          comment->type = kComment;

          output.status = kParseSuccess;
          output.parsed_length = content.length;
          output.result = comment;
        }
      }
    }
    return output;
  }

  ParseOutput  ParseParameter(Input input)
  {
    ParseOutput output;

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    ParseOutput name_parse_output = ConsumeByParserFunction(&input, &ParseToken);
    if (name_parse_output.status == kParseSuccess)
    {
      int parsed_length = ConsumeByCharacter(&input, '=');
      if (parsed_length > 0)
      {
        ParseOutput value_parse_output = ConsumeByParserFunction(&input, &ParseToken);
        if (value_parse_output.status != kParseSuccess)
        {
          value_parse_output = ConsumeByParserFunction(&input, &ParseQuotedString);
        }
        if (value_parse_output.parsed_length > 0)
        {
          PTNodeParameter*  parameter = PTNodeCreate<PTNodeParameter>();
          parameter->type = kParameter;
          parameter->name = static_cast<PTNodeToken*>(name_parse_output.result);
          parameter->value_header = value_parse_output.result;

          output.status = kParseSuccess;
          output.parsed_length = name_parse_output.parsed_length + parsed_length + value_parse_output.parsed_length;
          output.result = parameter;
        }
        else
        {
          temporary::arena.rollback(snapshot);
        }
      }
    }
    return output;
  }

  ParseOutput  ParseParameters(Input input)
  {
    ParseOutput output;
    int parsed_length = 0;
    temporary::vector<PTNodeParameter*> children;

    while (input.length > 0)
    {
      int error = true;
      int parsed_length_1 = ConsumeByScanFunction(&input, &ScanOptionalWhitespace).length;
      int parsed_length_2 = ConsumeByCharacter(&input, ';');
      if (parsed_length_2 > 0)
      {
        int parsed_length_3 = ConsumeByScanFunction(&input, &ScanOptionalWhitespace).length;
        ParseOutput parameter_parse_output = ConsumeByParserFunction(&input, &ParseParameter);
        if (parameter_parse_output.status == kParseSuccess)
        {
          parsed_length += parsed_length_1 + parsed_length_2 + parsed_length_3 + parameter_parse_output.parsed_length;
          children.push_back(static_cast<PTNodeParameter*>(parameter_parse_output.result));
          error = false;
        }
      }
      if (error)
        break;
    }
    if (children.size() > 0)
    {
      PTNodeParameters* parameters = PTNodeCreate<PTNodeParameters>();
      parameters->type = kParameters;
      parameters->children = children;

      output.status = kParseSuccess;
      output.parsed_length = parsed_length;
      output.result = parameters;
    }
    return output;
  }

  /////////////////////////////////////////
  ////////////////   path   ///////////////
  /////////////////////////////////////////

  bool  IsUnreservered(char character)
  {
    return (IsAlpha(character) ||
            IsDigit(character) ||
            std::strchr("-._~", character));
  }

  bool  IsReservered(char character)
  {
    return (IsGenDelims(character) ||
            IsSubDelims(character));
  }

  bool  IsGenDelims(char character)
  {
    return (std::strchr(":/?#[]", character));
  }

  bool  IsSubDelims(char character)
  {
    return (std::strchr("!$&'()*+,;=", character));
  }

  ScanOutput  ScanPathChar(Input input)
  {
    ScanOutput  output;
    if (input.is_valid())
    {
      if (input.length > 0 && 
          (IsUnreservered(*input.bytes) ||
          IsSubDelims(*input.bytes) ||
          *input.bytes == ':' ||
          *input.bytes == '@'))
      {
        output.bytes = input.bytes;
        output.length = 1;
      }
      else
      {
        output = ScanPctEncoded(input);
      }
    }
    return output;
  }

  ScanOutput  ScanPctEncoded(Input input)
  {
    ScanOutput  output;

    if (input.is_valid() && input.length >= 3)
    {
      output.bytes = input.bytes;
      if (*input.bytes == '%' &&
          IsHexDigit(*(input.bytes + 1)) &&
          IsHexDigit(*(input.bytes + 2)))
      {
        output.length = 3;
      }
    }
    return output;
  }

  ScanOutput  ScanSegment(Input input)
  {
    ScanOutput  output;

    if (input.is_valid())
    {
      output.bytes = input.bytes;
      int total_length = 0;
      ScanOutput scan_result = ConsumeByScanFunction(&input, &ScanPathChar);
      while (scan_result.is_valid())
      {
        total_length += scan_result.length;
        scan_result = ConsumeByScanFunction(&input, &ScanPathChar);
      }
      output.length = total_length;
    }
    return output;
  }

  ScanOutput  ScanSegments(Input input)
  {
    ScanOutput  output;

    if (input.is_valid())
    {
      output.bytes = input.bytes;
      int total_length = 0;
      while (true)
      {
        int parsed_length_1 = ConsumeByCharacter(&input, '/');
        int parsed_length_2 = 0;
        if (parsed_length_1 > 0)
        {
          parsed_length_2 = ConsumeByScanFunction(&input, &ScanSegment).length;
        }
        int segment_length = parsed_length_1 + parsed_length_2;
        if (segment_length > 0)
          total_length += segment_length;
        else
          break;
      }
      output.length = total_length;
    }
    return output;
  }

  ScanOutput  ScanSegmentNz(Input input)
  {
    ScanOutput  output;

    if (input.is_valid())
    {
      output.bytes = input.bytes;
      int total_length = 0;
      ScanOutput scan_result = ConsumeByScanFunction(&input, &ScanPathChar);
      while (scan_result.is_valid())
      {
        total_length += scan_result.length;
        scan_result = ConsumeByScanFunction(&input, &ScanPathChar);
      }
      if (total_length > 1)
        output.length = total_length;
      else
        output.bytes = NULL;
    }
    return output;
  }

  ScanOutput  ScanSegmentNzNc(Input input)
  {
    ScanOutput  output;

    if (input.is_valid())
    {
      output.bytes = input.bytes;
      int total_length = 0;
      int scan_length = 1;
      while (scan_length > 0)
      {
        scan_length = ConsumeByUnitFunction(&input, &IsUnreservered);
        if (scan_length == 0)
          scan_length = ConsumeByScanFunction(&input, &ScanPctEncoded).length;
        if (scan_length == 0)
          scan_length = ConsumeByUnitFunction(&input, &IsSubDelims);
        if (scan_length == 0)
          scan_length = ConsumeByCharacter(&input, '@');
        total_length += scan_length;
      }
      if (total_length > 1)
        output.length = total_length;
      else
        output.bytes = NULL;
    }
    return output;
  }

  ParseOutput  ParsePathAbEmpty(Input input)
  {
    ParseOutput output;
    StringSlice content = ConsumeByScanFunction(&input, &ScanSegments);
    if (output.parsed_length > 0)
    {
      PTNodePathAbEmpty*  path_abempty = PTNodeCreate<PTNodePathAbEmpty>();

      path_abempty->type = kPathAbempty;
      path_abempty->content = content;

      output.status = kParseSuccess;
      output.parsed_length = content.length;
      output.result = path_abempty;
    }
    return output;
  }

  ParseOutput  ParsePathAbsolute(Input input)
  {
    ParseOutput output;

    StringSlice content;
    content.bytes = input.bytes;
    content.length = ConsumeByCharacter(&input, '/');
    if (content.length > 0)
    {
      Input scoped_input = input;
      ScanOutput parsed_result = ConsumeByScanFunction(&scoped_input, &ScanSegmentNz);
      if (parsed_result.is_valid())
      {
        ScanOutput parsed_result_2 = ConsumeByScanFunction(&scoped_input, &ScanSegments);
        if (parsed_result_2.is_valid())
        {
          input = scoped_input;
          content.length += parsed_result.length + parsed_result_2.length;
        }
      }
      PTNodePathAbsolute* path_absolute = PTNodeCreate<PTNodePathAbsolute>();

      path_absolute->type = kPathAbsolute;
      path_absolute->content = content;

      output.status = kParseSuccess;
      output.parsed_length = content.length;
      output.result = path_absolute;
    }
    return output;
  }

  ParseOutput  ParsePathNoScheme(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ScanOutput  parsed_result = ConsumeByScanFunction(&input, &ScanSegmentNzNc);
    if (parsed_result.is_valid())
    {
      ScanOutput  parsed_result_2 = ConsumeByScanFunction(&input, &ScanSegments);
      if (parsed_result_2.is_valid())
      {
        PTNodePathNoScheme* path_noscheme = PTNodeCreate<PTNodePathNoScheme>();

        path_noscheme->type = kPathNoScheme;
        path_noscheme->content = StringSlice(input_start, parsed_result.length + parsed_result_2.length);;

        output.status = kParseSuccess;
        output.parsed_length = path_noscheme->content.length;
        output.result = path_noscheme;
      }
    }
    return output;
  }

  ParseOutput  ParsePathRootless(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ScanOutput  parsed_result = ConsumeByScanFunction(&input, &ScanSegmentNz);
    if (parsed_result.is_valid())
    {
      ScanOutput  parsed_result_2 = ConsumeByScanFunction(&input, &ScanSegments);
      if (parsed_result_2.is_valid())
      {
        PTNodePathRootless* path_rootless = PTNodeCreate<PTNodePathRootless>();

        path_rootless->type = kPathRootless;
        path_rootless->content = StringSlice(input_start, parsed_result.length + parsed_result_2.length);;

        output.status = kParseSuccess;
        output.parsed_length = path_rootless->content.length;
        output.result = path_rootless;
      }
    }
    return output;
  }

  ParseOutput  ParsePathEmpty(Input)
  {
    ParseOutput output;
    output.status = kParseSuccess;
    output.parsed_length = 0;
    output.result = static_cast<PTNode*>(PTNodeCreate<PTNodePathEmpty>());
    output.result->type = kPathEmpty;
    return output;
  }

  ////////////////////////////////////////////////
  ////////////////   ip address   ////////////////
  ////////////////////////////////////////////////

  ScanOutput   ScanDecOctet(Input input)
  {
    ScanOutput  output;

    if (input.is_valid() &&
       (input.length > 0))
    {
      output.bytes = input.bytes;
      int scan_length = 0;
      if (input.bytes[0] == '0')
        scan_length = 1;
      else
      {
        int num = 0;
        const char* start = input.bytes;
        const char* last = input.length > 3 ? input.bytes + 3 : input.bytes + input.length;
        for (; start < last; start++)
        {
          if (IsDigit(*start))
          {
           num = num * 10 + (*start - '0');
          }
          else
            break;
        }
        if ((start != input.bytes) && (num >= 0) && (num <= 255))
        {
          scan_length = start - input.bytes;
        }
      }
      if (scan_length > 0)
      {
        output.bytes = input.bytes;
        output.length = scan_length;
      }
    }
    return output;
  }

  ScanOutput  ScanH16(Input input)
  {
    ScanOutput  output;

    if (input.is_valid() &&
       (input.length > 0))
    {
      int scan_length = 0;
      for (int i = 0; i < 4; i++)
      {
        if (IsHexDigit(input.bytes[i]))
          scan_length++;
        else
          break;
      }
      if (scan_length > 0)
      {
        output.bytes = input.bytes;
        output.length = scan_length;
      }
    }
    return output;
  }

  ParseOutput  ParseIpv6Address(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    int amount_of_optional_h16 = 0;
    {
      while (input.length > 0)
      {
        Input input_tmp = input;
        ScanOutput parsed_h16 = ConsumeByScanFunction(&input_tmp, &ScanH16);
        if (!parsed_h16.is_valid() ||
            (ConsumeByCharacter(&input_tmp, ':') == 0) ||
            (input_tmp.length > 0 && (*input_tmp.bytes == ':'))) // cannot be a ::
          break;
        amount_of_optional_h16++;
        input = input_tmp;
      }
      if (ConsumeByScanFunction(&input, &ScanH16).is_valid())
        amount_of_optional_h16++;
    }
    ArenaSnapshot snapshot = temporary::arena.snapshot();
    if (!(amount_of_optional_h16 == 6 &&
            (ConsumeByParserFunction(&input, &ParseIpv4Address).status == kParseSuccess)) &&
         (amount_of_optional_h16 != 8))
    {
      temporary::arena.rollback(snapshot);
      if (ConsumeByCString(&input, "::") == 0)
        return output;
      int max_amount_of_h16 = 7 - amount_of_optional_h16;
      int amount_of_h16 = 0;
      while (input.length > 0 && (amount_of_h16 <= max_amount_of_h16))
      {
        Input input_tmp = input;
        ScanOutput parsed_h16 = ConsumeByScanFunction(&input_tmp, &ScanH16);
        if (!parsed_h16.is_valid() ||
            (ConsumeByCharacter(&input_tmp, ':') == 0))
          break;
        amount_of_h16++;
        input = input_tmp;
      }
      bool  should_scan_h16 = true;
      if ((max_amount_of_h16 - amount_of_h16) >= 2)
      {
        if(ConsumeByParserFunction(&input, &ParseIpv4Address).status == kParseSuccess)
          should_scan_h16 = false;
        temporary::arena.rollback(snapshot);
      }
      if (should_scan_h16)
      {
        if (!ConsumeByScanFunction(&input, &ScanH16).is_valid())
          return output;
      }
    }
    PTNodeIpv6Address* ipv6_address = PTNodeCreate<PTNodeIpv6Address>(); 
    ipv6_address->type = kIpv6Address;
    ipv6_address->content = StringSlice(input_start, input.bytes - input_start);

    output.status = kParseSuccess;
    output.parsed_length = ipv6_address->content.length;
    output.result = ipv6_address;
    return output;
  }

  ParseOutput  ParseIpvFutureAddress(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if (ConsumeByCharacter(&input, 'v') != 1)
      return output;
    int amount = 0;
    while (input.length > 0)
    {
      if (ConsumeByUnitFunction(&input, &IsHexDigit) == 1)
        amount++;
      else
        break;
    }
    if (amount < 1)
      return output;
    if (ConsumeByCharacter(&input, '.') != 1)
      return output;
    int amount_2 = 0;
    while (input.length > 0)
    {
      if ((ConsumeByUnitFunction(&input, &IsUnreservered) == 0) &&
          (ConsumeByUnitFunction(&input, &IsSubDelims) == 0) &&
          (ConsumeByCharacter(&input, ':') == 0))
        break;
      else
        amount_2++;
    }
    if (amount_2 < 1)
      return output;

    PTNodeIpvFutureAddress* ipv_future_address = PTNodeCreate<PTNodeIpvFutureAddress>(); 
    ipv_future_address->type = kIpvFutureAddress;
    ipv_future_address->content = StringSlice(input_start, input.bytes - input_start);

    output.status = kParseSuccess;
    output.parsed_length = ipv_future_address->content.length;
    output.result = ipv_future_address;
    return output;
  }

  ParseOutput  ParseIpv4Address(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ScanOutput  parsed_octet_1 = ConsumeByScanFunction(&input, &ScanDecOctet);
    if (!parsed_octet_1.is_valid())
      return output;
    if (ConsumeByCharacter(&input, '.') != 1)
      return output;
    ScanOutput parsed_octet_2 = ConsumeByScanFunction(&input, &ScanDecOctet);
    if (!parsed_octet_2.is_valid())
      return output;
    if (ConsumeByCharacter(&input, '.') != 1)
      return output;
    ScanOutput parsed_octet_3 = ConsumeByScanFunction(&input, &ScanDecOctet);
    if (!parsed_octet_3.is_valid())
      return output;
    if (ConsumeByCharacter(&input, '.') != 1)
      return output;
    ScanOutput parsed_octet_4 = ConsumeByScanFunction(&input, &ScanDecOctet);
    if (!parsed_octet_4.is_valid())
      return output;

    PTNodeIpv4Address* ipv4_address = PTNodeCreate<PTNodeIpv4Address>(); 
    ipv4_address->type = kIpv4Address;
    ipv4_address->content = StringSlice(input_start, parsed_octet_1.length + parsed_octet_2.length + parsed_octet_3.length + parsed_octet_4.length + 3); // 3 is the length of the 3 dots.

    output.status = kParseSuccess;
    output.parsed_length = ipv4_address->content.length;
    output.result = ipv4_address;
    return output;
  }

  ParseOutput  ParseRegName(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    while (input.length > 0)
    {
      if ((ConsumeByUnitFunction(&input, &IsUnreservered) != 1) &&
          !ConsumeByScanFunction(&input, &ScanPctEncoded).is_valid() &&
          (ConsumeByUnitFunction(&input, &IsSubDelims) != 1))
        break;
    }
    PTNodeRegName* reg_name = PTNodeCreate<PTNodeRegName>(); 
    reg_name->type = kRegName;
    reg_name->content = StringSlice(input_start, input.bytes - input_start);

    output.status = kParseSuccess;
    output.parsed_length = reg_name->content.length;
    output.result = reg_name;

    return output;
  }

  /////////////////////////////////////////
  ////////////////   uri   ////////////////
  /////////////////////////////////////////

  ParseOutput  ParseUriScheme(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if (ConsumeByUnitFunction(&input, &IsAlpha) == 0)
      return output;
    while (input.length > 0)
    {
      if (!IsAlpha(*input.bytes) &&
          !IsDigit(*input.bytes) &&
          !std::strchr("+=.", *input.bytes))
        break;
      else
        input.consume();
    }
    PTNodeUriScheme* scheme = PTNodeCreate<PTNodeUriScheme>(); 
    scheme->type = kUriScheme;
    scheme->content = StringSlice(input_start, input.bytes - input_start);

    output.status = kParseSuccess;
    output.parsed_length = scheme->content.length;
    output.result = scheme;
    return output;
  }

  ParseOutput  ParseUriHost(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    ParseOutput parsed_address;

    {
      Input input_tmp = input;
      if (ConsumeByCharacter(&input_tmp, '[') == 1)
      {
        parsed_address = ConsumeByParserFunction(&input_tmp, &ParseIpv6Address);
        if (parsed_address.status != kParseSuccess)
          parsed_address = ConsumeByParserFunction(&input_tmp, &ParseIpvFutureAddress);

        if (parsed_address.status == kParseSuccess)
        {
          if (ConsumeByCharacter(&input_tmp, ']') == 0)
            parsed_address = ParseOutput();
          else
            input = input_tmp;
        }
      }
    }
    if (parsed_address.status != kParseSuccess)
      parsed_address = ConsumeByParserFunction(&input, &ParseIpv4Address);
    if (parsed_address.status != kParseSuccess)
      parsed_address = ConsumeByParserFunction(&input, &ParseRegName);

    if (parsed_address.status == kParseSuccess)
    {
      PTNodeUriHost* host = PTNodeCreate<PTNodeUriHost>(); 
      host->type = kUriHost;
      host->address_header = parsed_address.result;

      output.status = kParseSuccess;
      output.parsed_length = input.bytes - input_start;
      output.result = host;
    }
    return output;
  }

  ParseOutput  ParseUriPort(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    int number = 0;

    while ((input.length > 0) && IsDigit(*input.bytes))
    {
      number = number * 10 + (*input.bytes - '0');
      input.consume();
    }

    PTNodeUriPort* port = PTNodeCreate<PTNodeUriPort>(); 
    port->type = kUriPort;
    port->number = number;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = port;
    return output;
  }

  ParseOutput  ParseUriQuery(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    while (ConsumeByScanFunction(&input, &ScanPathChar).is_valid() ||
           ConsumeByCharacter(&input, '/') ||
           ConsumeByCharacter(&input, '?'))
      ;

    PTNodeUriQuery* query = PTNodeCreate<PTNodeUriQuery>(); 
    query->type = kUriQuery;
    query->content = StringSlice(input_start, input.bytes - input_start);

    output.status = kParseSuccess;
    output.parsed_length = query->content.length;
    output.result = query;
    return output;
  }

  ParseOutput  ParseUriUserInfo(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    while (ConsumeByUnitFunction(&input, &IsUnreservered) ||
           ConsumeByScanFunction(&input, &ScanPctEncoded).is_valid() ||
           ConsumeByUnitFunction(&input, &IsSubDelims) ||
           ConsumeByCharacter(&input, ';'))
      ;

    PTNodeUriUserInfo* user_info = PTNodeCreate<PTNodeUriUserInfo>(); 
    user_info->type = kUriUserInfo;
    user_info->content = StringSlice(input_start, input.bytes - input_start);

    output.status = kParseSuccess;
    output.parsed_length = user_info->content.length;
    output.result = user_info;
    return output;
  }

  ParseOutput  ParseUriAuthority(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNodeUriUserInfo*  user_info = NULL;
    PTNodeUriHost*      host = NULL;
    PTNodeUriQuery*     query = NULL;

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    {
      Input input_tmp = input;
      ParseOutput parsed_user_info = ConsumeByParserFunction(&input_tmp, &ParseUriUserInfo);
      if (parsed_user_info.status == kParseSuccess)
      {
        if (ConsumeByCharacter(&input_tmp, '@'))
        {
          user_info = static_cast<PTNodeUriUserInfo*>(parsed_user_info.result);
          input = input_tmp;
        }
        else
          temporary::arena.rollback(snapshot);
      }
    }
    ParseOutput parsed_host = ConsumeByParserFunction(&input, &ParseUriHost);
    if (parsed_host.status == kParseSuccess)
      host = static_cast<PTNodeUriHost*>(parsed_host.result);
    else
    {
      temporary::arena.rollback(snapshot);
      return output;
    }
    {
      Input input_tmp = input;
      if (ConsumeByCharacter(&input_tmp, ':'))
      {
        ParseOutput parsed_query = ConsumeByParserFunction(&input_tmp, &ParseUriQuery);
        if (parsed_query.status == kParseSuccess)
        {
          query = static_cast<PTNodeUriQuery*>(parsed_query.result);
          input = input_tmp;
        }
      }
    }
    PTNodeUriAuthority* authority = PTNodeCreate<PTNodeUriAuthority>(); 
    authority->type = kUriAuthority;
    authority->user_info = user_info;
    authority->host = host;
    authority->query = query;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = authority;
    return output;
  }

  ParseOutput  ParseUriFragment(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    while (ConsumeByScanFunction(&input, &ScanPathChar).is_valid() ||
           ConsumeByCharacter(&input, '/') ||
           ConsumeByCharacter(&input, '?'))
      ;

    PTNodeUriFragment* fragment = PTNodeCreate<PTNodeUriFragment>(); 
    fragment->type = kUriFragment;
    fragment->content = StringSlice(input_start, input.bytes - input_start);

    output.status = kParseSuccess;
    output.parsed_length = fragment->content.length;
    output.result = fragment;
    return output;
  }

  ParseOutput  ParseUriReferenceNetworkPath(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNodeUriAuthority* authority = NULL;
    PTNodePathAbEmpty*  path = NULL;

    if (ConsumeByCString(&input, "//") == 0)
      return output;
    ArenaSnapshot snapshot = temporary::arena.snapshot();
    {
      ParseOutput parsed_authority = ConsumeByParserFunction(&input, &ParseUriAuthority);
      if (parsed_authority.status != kParseSuccess)
        return output;
      authority = static_cast<PTNodeUriAuthority*>(parsed_authority.result);
    }
    {
      ParseOutput parsed_path_abempty = ConsumeByParserFunction(&input, &ParsePathAbEmpty);
      if (parsed_path_abempty.status != kParseSuccess)
      {
        temporary::arena.rollback(snapshot);
        return output;
      }
      path = static_cast<PTNodePathAbEmpty*>(parsed_path_abempty.result);
    }

    PTNodeUriReferenceNetworkPath* reference_network_path = PTNodeCreate<PTNodeUriReferenceNetworkPath>(); 
    reference_network_path->type = kUriReferenceNetworkPath;
    reference_network_path->authority = authority;
    reference_network_path->path = path;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = reference_network_path;
    return output;
  }

  ParseOutput  ParseUri(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNodeUriScheme*   scheme = NULL;
    PTNode*            path_header = NULL;
    PTNodeUriQuery*    query = NULL;
    PTNodeUriFragment* fragment = NULL;

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    {
      ParseOutput parsed_scheme = ConsumeByParserFunction(&input, &ParseUriScheme);
      if (parsed_scheme.status == kParseSuccess)
        scheme = static_cast<PTNodeUriScheme*>(parsed_scheme.result);
      else
        return output;
    }
    if (ConsumeByCharacter(&input, ':') == 0)
    {
      temporary::arena.rollback(snapshot);
      return output;
    }
    {
      ParseOutput parsed_path_header = ConsumeByParserFunction(&input, &ParseUriReferenceNetworkPath);
      if (parsed_path_header.status == kParseSuccess)
        path_header = parsed_path_header.result;
      else
      {
        parsed_path_header = ConsumeByParserFunction(&input, &ParsePathAbsolute);
        if (parsed_path_header.status == kParseSuccess)
          path_header = parsed_path_header.result;
        else
        {
          parsed_path_header = ConsumeByParserFunction(&input, &ParsePathRootless);
          if (parsed_path_header.status == kParseSuccess)
            path_header = parsed_path_header.result;
          else
          {
            parsed_path_header = ConsumeByParserFunction(&input, &ParsePathEmpty);
            if (parsed_path_header.status == kParseSuccess)
              path_header = parsed_path_header.result;
            else
            {
              temporary::arena.rollback(snapshot);
              return output;
            }
          }
        }
      }
    }
    {
      Input input_tmp = input;
      if (ConsumeByCharacter(&input_tmp, '?') == 1)
      {
        ParseOutput parsed_query = ConsumeByParserFunction(&input_tmp, &ParseUriQuery);
        if (parsed_query.status == kParseSuccess)
        {
          query = static_cast<PTNodeUriQuery*>(parsed_query.result);
          input = input_tmp;
        }
      }
    }
    {
      Input input_tmp = input;
      if (ConsumeByCharacter(&input_tmp, '#') == 1)
      {
        ParseOutput parsed_query = ConsumeByParserFunction(&input_tmp, &ParseUriFragment);
        if (parsed_query.status == kParseSuccess)
        {
          fragment = static_cast<PTNodeUriFragment*>(parsed_query.result);
          input = input_tmp;
        }
      }
    }

    PTNodeUri* uri = PTNodeCreate<PTNodeUri>(); 
    uri->type = kUri;
    uri->scheme = scheme;
    uri->path_header = path_header;
    uri->query = query;
    uri->fragment = fragment;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = uri;
    return output;
  }

  ParseOutput  ParseUriAbsolute(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNodeUriScheme*   scheme = NULL;
    PTNode*            path_header = NULL;
    PTNodeUriQuery*    query = NULL;

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    {
      ParseOutput parsed_scheme = ConsumeByParserFunction(&input, &ParseUriScheme);
      if (parsed_scheme.status == kParseSuccess)
        scheme = static_cast<PTNodeUriScheme*>(parsed_scheme.result);
      else
        return output;
    }
    if (ConsumeByCharacter(&input, ':') == 0)
    {
      temporary::arena.rollback(snapshot);
      return output;
    }
    {
      ParseOutput parsed_path_header = ConsumeByParserFunction(&input, &ParseUriReferenceNetworkPath);
      if (parsed_path_header.status == kParseSuccess)
        path_header = parsed_path_header.result;
      else
      {
        parsed_path_header = ConsumeByParserFunction(&input, &ParsePathAbsolute);
        if (parsed_path_header.status == kParseSuccess)
          path_header = parsed_path_header.result;
        else
        {
          parsed_path_header = ConsumeByParserFunction(&input, &ParsePathRootless);
          if (parsed_path_header.status == kParseSuccess)
            path_header = parsed_path_header.result;
          else
          {
            parsed_path_header = ConsumeByParserFunction(&input, &ParsePathEmpty);
            if (parsed_path_header.status == kParseSuccess)
              path_header = parsed_path_header.result;
            else
            {
              temporary::arena.rollback(snapshot);
              return output;
            }
          }
        }
      }
    }
    {
      Input input_tmp = input;
      if (ConsumeByCharacter(&input_tmp, '?') == 1)
      {
        ParseOutput parsed_query = ConsumeByParserFunction(&input_tmp, &ParseUriQuery);
        if (parsed_query.status == kParseSuccess)
        {
          query = static_cast<PTNodeUriQuery*>(parsed_query.result);
          input = input_tmp;
        }
      }
    }
    PTNodeUriAbsolute* url_absolute = PTNodeCreate<PTNodeUriAbsolute>(); 
    url_absolute->type = kUriAbsolute;
    url_absolute->scheme = scheme;
    url_absolute->path_header = path_header;
    url_absolute->query = query;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = url_absolute;
    return output;
  }

  ParseOutput  ParseUriReferenceRelative(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNode*            path_header = NULL;
    PTNodeUriQuery*    query = NULL;
    PTNodeUriFragment* fragment = NULL;

    {
      ParseOutput parsed_path_header = ConsumeByParserFunction(&input, &ParseUriReferenceNetworkPath);
      if (parsed_path_header.status == kParseSuccess)
        path_header = parsed_path_header.result;
      else
      {
        parsed_path_header = ConsumeByParserFunction(&input, &ParsePathAbsolute);
        if (parsed_path_header.status == kParseSuccess)
          path_header = parsed_path_header.result;
        else
        {
          parsed_path_header = ConsumeByParserFunction(&input, &ParsePathNoScheme);
          if (parsed_path_header.status == kParseSuccess)
            path_header = parsed_path_header.result;
          else
          {
            parsed_path_header = ConsumeByParserFunction(&input, &ParsePathEmpty);
            if (parsed_path_header.status == kParseSuccess)
              path_header = parsed_path_header.result;
            else
              return output;
          }
        }
      }
    }
    {
      Input input_tmp = input;
      if (ConsumeByCharacter(&input_tmp, '?') == 1)
      {
        ParseOutput parsed_query = ConsumeByParserFunction(&input_tmp, &ParseUriQuery);
        if (parsed_query.status == kParseSuccess)
        {
          query = static_cast<PTNodeUriQuery*>(parsed_query.result);
          input = input_tmp;
        }
      }
    }
    {
      Input input_tmp = input;
      if (ConsumeByCharacter(&input_tmp, '#') == 1)
      {
        ParseOutput parsed_query = ConsumeByParserFunction(&input_tmp, &ParseUriFragment);
        if (parsed_query.status == kParseSuccess)
        {
          fragment = static_cast<PTNodeUriFragment*>(parsed_query.result);
          input = input_tmp;
        }
      }
    }

    PTNodeUriReferenceRelative* uri_reference_relative = PTNodeCreate<PTNodeUriReferenceRelative>(); 
    uri_reference_relative->type = kUriReferenceRelative;
    uri_reference_relative->path_header = path_header;
    uri_reference_relative->query = query;
    uri_reference_relative->fragment = fragment;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = uri_reference_relative;
    return output;
  }

  ParseOutput  ParseUriReference(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNode* uri_header = NULL;

    ParseOutput parsed_uri_header = ConsumeByParserFunction(&input, &ParseUri);
    if (parsed_uri_header.status == kParseSuccess)
      uri_header = parsed_uri_header.result;
    else
    {
      parsed_uri_header = ConsumeByParserFunction(&input, &ParseUriReferenceRelative);
      if (parsed_uri_header.status == kParseSuccess)
        uri_header = parsed_uri_header.result;
      else
        return output;
    }

    PTNodeUriReference* uri_reference = PTNodeCreate<PTNodeUriReference>(); 
    uri_reference->type = kUriReference;
    uri_reference->uri_header = uri_header;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = uri_reference;
    return output;
  }

  //////////////////////////////////////////////////
  ////////////////   Request line   ////////////////
  //////////////////////////////////////////////////

  // Although the status-line grammar rule requires that each of the component elements be separated by a single SP octet, recipients MAY instead parse on whitespace-delimited word boundaries and, aside from the line terminator, treat any form of whitespace as the SP separator while ignoring preceding or trailing whitespace; such whitespace includes one or more of the following octets: SP, HTAB, VT (%x0B), FF (%x0C), or bare CR. However, lenient parsing can result in response splitting security vulnerabilities if there are multiple recipients of the message and each has its own unique interpretation of robustness.
  ScanOutput  ScanStartLineSpaces(Input input)
  {
    ScanOutput output;
    if (input.is_valid())
    {
      int scan_length = 0;
      while ((input.length > 0) &&
            (IsSpace(*input.bytes) ||
             IsHorizontalTab(*input.bytes) ||
             (*input.bytes == 11) || // vertical tab
             (*input.bytes == 12) || // NP form feed, new page
             IsCarriageReturn(*input.bytes)))
      {
        scan_length++;
      }
      if (scan_length > 0)
      {
        output.bytes = input.bytes;
        output.length = scan_length;
      }
    }
    return output;
  }

  ParseOutput  ParseHttpVersion(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if ((ConsumeByCString(&input, "HTTP") == 4) &&
        (ConsumeByCharacter(&input, '/') == 1) &&
        (ConsumeByUnitFunction(&input, &IsDigit) == 1) &&
        (ConsumeByCharacter(&input, '.') == 1) &&
        (ConsumeByUnitFunction(&input, &IsDigit) == 1))
    {
      PTNodeHttpVersion*  http_version = PTNodeCreate<PTNodeHttpVersion>();
      http_version->type = kHttpVersion;
      http_version->content = StringSlice(input_start, input.bytes - input_start);

      output.status = kParseSuccess;
      output.parsed_length = input.bytes - input_start;
      output.result = http_version;
    }
    return output;
  }

  ParseOutput  ParseMethod(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if ((ConsumeByCString(&input, "GET") == 3) ||
        (ConsumeByCString(&input, "HEAD") == 4) ||
        (ConsumeByCString(&input, "POST") == 4) ||
        (ConsumeByCString(&input, "PUT") == 3) ||
        (ConsumeByCString(&input, "DELETE") == 6) ||
        (ConsumeByCString(&input, "CONNECT") == 7) ||
        (ConsumeByCString(&input, "OPTIONS") == 7) ||
        (ConsumeByCString(&input, "TRACE") == 5))
    {
      PTNodeMethod*  method = PTNodeCreate<PTNodeMethod>();
      method->type = kMethod;
      method->content = StringSlice(input_start, input.bytes - input_start);

      output.status = kParseSuccess;
      output.parsed_length = input.bytes - input_start;
      output.result = method;
    }
    return output;
  }

  ParseOutput  ParseRequestTargetOriginForm(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNodePathAbsolute* absolute_path = NULL;
    PTNodeUriQuery*     query = NULL;

    {
      ParseOutput parsed_absolute_path = ConsumeByParserFunction(&input, &ParsePathAbsolute);
      if (parsed_absolute_path.status == kParseSuccess)
        absolute_path = static_cast<PTNodePathAbsolute*>(parsed_absolute_path.result);
      else
        return output;
    }
    {
      Input input_tmp = input;
      if (ConsumeByCharacter(&input_tmp, '?') == 1)
      {
        ParseOutput parsed_query = ConsumeByParserFunction(&input_tmp, &ParseUriQuery);
        if (parsed_query.status == kParseSuccess)
        {
          query = static_cast<PTNodeUriQuery*>(parsed_query.result);
          input = input_tmp;
        }
      }
    }

    PTNodeRequestTargetOriginForm*  request_target_origin_form = PTNodeCreate<PTNodeRequestTargetOriginForm>();
    request_target_origin_form->type = kRequestTargetOriginForm;
    request_target_origin_form->absolute_path = absolute_path;
    request_target_origin_form->query = query;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = request_target_origin_form;
    return output;
  }

  ParseOutput  ParseRequestTargetAuthorityForm(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNodeUriHost* host = NULL;
    PTNodeUriPort* port = NULL;

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    {
      ParseOutput parsed_host = ConsumeByParserFunction(&input, &ParseUriHost);
      if (parsed_host.status == kParseSuccess)
        host = static_cast<PTNodeUriHost*>(parsed_host.result);
      else
        return output;
    }
    if (ConsumeByCharacter(&input, ':') == 0)
    {
      temporary::arena.rollback(snapshot);
      return output;
    }
    {
      ParseOutput parsed_host = ConsumeByParserFunction(&input, &ParseUriPort);
      if (parsed_host.status == kParseSuccess)
        port = static_cast<PTNodeUriPort*>(parsed_host.result);
      else
      {
        temporary::arena.rollback(snapshot);
        return output;
      }
    }

    PTNodeRequestTargetAuthorityForm*  request_target_authority_form = PTNodeCreate<PTNodeRequestTargetAuthorityForm>();
    request_target_authority_form->type = kRequestTargetAuthorityForm;
    request_target_authority_form->host = host;
    request_target_authority_form->port = port;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = request_target_authority_form;
    return output;
  }

  ParseOutput  ParseRequestTargetAsteriskForm(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if (ConsumeByCharacter(&input, '*') == 1)
    {
      PTNodeRequestTargetAsteriskForm*  request_target_asterisk_form = PTNodeCreate<PTNodeRequestTargetAsteriskForm>();
      request_target_asterisk_form->type = kRequestAsteriskForm;

      output.status = kParseSuccess;
      output.parsed_length = input.bytes - input_start;
      output.result = request_target_asterisk_form;
    }
    return output;
  }

  ParseOutput  ParseRequestLine(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNodeMethod*       method = NULL;
    PTNode*             request_target_header = NULL;
    PTNodeHttpVersion*  version = NULL;

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    {
      ParseOutput parsed_method = ConsumeByParserFunction(&input, &ParseMethod);
      if (parsed_method.status == kParseSuccess)
        method = static_cast<PTNodeMethod*>(parsed_method.result);
      else
        return output;
    }
    if (!ConsumeByScanFunction(&input, &ScanStartLineSpaces).is_valid())
    {
      temporary::arena.rollback(snapshot);
      return output;
    }
    {
      ParseOutput parsed_request_target = ConsumeByParserFunction(&input, &ParseRequestTargetOriginForm);
      if (parsed_request_target.status == kParseSuccess)
        request_target_header = parsed_request_target.result;
      else
      {
        parsed_request_target = ConsumeByParserFunction(&input, &ParseUriAbsolute);
        if (parsed_request_target.status == kParseSuccess)
          request_target_header = parsed_request_target.result;
        else
        {
          parsed_request_target = ConsumeByParserFunction(&input, &ParseRequestTargetAuthorityForm);
          if (parsed_request_target.status == kParseSuccess)
            request_target_header = parsed_request_target.result;
          else
          {
            parsed_request_target = ConsumeByParserFunction(&input, &ParseRequestTargetAsteriskForm);
            if (parsed_request_target.status == kParseSuccess)
              request_target_header = parsed_request_target.result;
            else
            {
              temporary::arena.rollback(snapshot);
              return output;
            }
          }
        }
      }
    }
    if (!ConsumeByScanFunction(&input, &ScanStartLineSpaces).is_valid())
    {
      temporary::arena.rollback(snapshot);
      return output;
    }
    {
      ParseOutput parsed_http_version = ConsumeByParserFunction(&input, &ParseHttpVersion);
      if (parsed_http_version.status == kParseSuccess)
        version = static_cast<PTNodeHttpVersion*>(parsed_http_version.result);
      else
      {
        temporary::arena.rollback(snapshot);
        return output;
      }
    }

    PTNodeRequestLine*  request_line = PTNodeCreate<PTNodeRequestLine>();
    request_line->type = kRequestLine;
    request_line->method = method;
    request_line->request_target_header = request_target_header;
    request_line->version = version;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = request_line;
    return output;
  }

  /////////////////////////////////////////////////
  ////////////////   status line   ////////////////
  /////////////////////////////////////////////////

  ParseOutput  ParseReasonPhrase(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    while (input.length > 0)
    {
      if (IsHorizontalTab(*input.bytes) ||
          IsSpace(*input.bytes) ||
          IsVisibleCharacter(*input.bytes) ||
          IsOpaqueText(*input.bytes))
        input.consume();
      else
        break;
    }
    if ((input.bytes - input_start) > 0)
    {
      PTNodeReasonPhrase*  reason_phrase = PTNodeCreate<PTNodeReasonPhrase>();
      reason_phrase->type = kStatusReasonPhrase;
      reason_phrase->content = StringSlice(input_start, input.bytes - input_start);

      output.status = kParseSuccess;
      output.parsed_length = input.bytes - input_start;
      output.result = reason_phrase;
    }
    return output;
  }

  ParseOutput  ParseStatusCode(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    int amount = 0;
    int number = 0;
    while ((amount < 3) && IsDigit(*input.bytes))
    {
      number = number * 10 + (*input.bytes - '0');
      input.consume();
      amount++;
    }
    if (amount == 3)
    {
      PTNodeStatusCode*  status_code = PTNodeCreate<PTNodeStatusCode>();
      status_code->type = kStatusCode;
      status_code->number  = number;

      output.status = kParseSuccess;
      output.parsed_length = input.bytes - input_start;
      output.result = status_code;
    }
    return output;
  }

  ParseOutput  ParseStatusLine(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNodeHttpVersion*  http_version = NULL;
    PTNodeStatusCode*   status_code = NULL;
    PTNodeReasonPhrase* reason_phrase = NULL;

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    {
      ParseOutput parsed_http_version = ConsumeByParserFunction(&input, &ParseHttpVersion);
      if (parsed_http_version.status == kParseSuccess)
        http_version = static_cast<PTNodeHttpVersion*>(parsed_http_version.result);
      else
        return output;
    }
    if (!ConsumeByScanFunction(&input, &ScanStartLineSpaces).is_valid())
    {
      temporary::arena.rollback(snapshot);
      return output;
    }
    {
      ParseOutput parsed_status_code = ConsumeByParserFunction(&input, &ParseStatusCode);
      if (parsed_status_code.status == kParseSuccess)
        status_code = static_cast<PTNodeStatusCode*>(parsed_status_code.result);
      else
      {
        temporary::arena.rollback(snapshot);
        return output;
      }
    }
    if (!ConsumeByScanFunction(&input, &ScanStartLineSpaces).is_valid())
    {
      temporary::arena.rollback(snapshot);
      return output;
    }
    {
      ParseOutput parsed_reason_phrase = ConsumeByParserFunction(&input, &ParseReasonPhrase);
      if (parsed_reason_phrase.status == kParseSuccess)
        reason_phrase = static_cast<PTNodeReasonPhrase*>(parsed_reason_phrase.result);
    }
    PTNodeStatusLine*  status_line = PTNodeCreate<PTNodeStatusLine>();
    status_line->type = kStatusLine;
    status_line->http_version = http_version;
    status_line->status_code = status_code;
    status_line->reason_phrase = reason_phrase;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = status_line;
    return output;
  }

  ////////////////////////////////////////////
  ////////////////   fields   ////////////////
  ////////////////////////////////////////////

  ParseOutput  ParseFieldValue(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    while (ConsumeByUnitFunction(&input, &IsWhitespace) == 1)
      ;
    const char* value_start = input.bytes;
    while (input.length > 0)
    {
      if (IsVisibleCharacter(*input.bytes) ||
          IsOpaqueText(*input.bytes))
      {
        input.consume();
        Input input_tmp = input;
        int amount = 0;
        while (input_tmp.length > 0)
        {
          if (IsSpace(*input_tmp.bytes) ||
              IsHorizontalTab(*input_tmp.bytes) ||
              IsVisibleCharacter(*input_tmp.bytes) ||
              IsOpaqueText(*input_tmp.bytes))
          {
             input_tmp.consume(); 
             amount++;
          }
          else
            break;
        }
        if ((amount > 0) && (input_tmp.length > 0) &&
            (IsVisibleCharacter(*input_tmp.bytes) ||
             IsOpaqueText(*input_tmp.bytes)))
          input = input_tmp;
      }
      else
        break;
    }
    const char* value_end = input.bytes;
    // remove trailing white space
    if ((value_end - value_start) > 0)
    {
      while ((value_end > value_start) && IsWhitespace(*(value_end - 1)))
      {
        value_end--;
      }
    }
    PTNodeFieldValue*  field_value = PTNodeCreate<PTNodeFieldValue>();
    field_value->type = kFieldValue;
    field_value->content = StringSlice(value_start, value_end - value_start);

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = field_value;
    return output;
  }

  ParseOutput  ParseFieldLine(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNodeToken*       name = NULL;
    PTNodeFieldValue*  value = NULL;

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    {
      ParseOutput parsed_field_name = ConsumeByParserFunction(&input, &ParseToken);
      if (parsed_field_name.status == kParseSuccess)
        name = static_cast<PTNodeToken*>(parsed_field_name.result);
      else
        return output;
    }
    if (!ConsumeByCharacter(&input, ':'))
    {
      temporary::arena.rollback(snapshot);
      return output;
    }
    ConsumeByScanFunction(&input, &ScanOptionalWhitespace);
    {
      ParseOutput parsed_field_value = ConsumeByParserFunction(&input, &ParseFieldValue);
      if (parsed_field_value.status == kParseSuccess)
        value = static_cast<PTNodeFieldValue*>(parsed_field_value.result);
      else
      {
        temporary::arena.rollback(snapshot);
        return output;
      }
    }
    ConsumeByScanFunction(&input, &ScanOptionalWhitespace);

    PTNodeFieldLine*  field_line = PTNodeCreate<PTNodeFieldLine>();
    field_line->type = kFieldLine;
    field_line->name = name;
    field_line->value = value;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = field_line;
    return output;
  }

  ParseOutput  ParseFields(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    temporary::vector<PTNodeFieldLine*> fields;

    while (input.length > 0)
    {
      ArenaSnapshot snapshot = temporary::arena.snapshot();
      ParseOutput parsed_field = ConsumeByParserFunction(&input, &ParseFieldLine);
      if (parsed_field.status == kParseSuccess)
      {
        if (ConsumeByScanFunction(&input, &ScanNewLine).is_valid())
          fields.push_back(static_cast<PTNodeFieldLine*>(parsed_field.result));
        else
        {
          temporary::arena.rollback(snapshot);
          break;
        }
      }
      else
        break;
    }

    PTNodeFields*  node_fields = PTNodeCreate<PTNodeFields>();
    node_fields->type = kFields;
    node_fields->fields = fields;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = node_fields;
    return output;
  }

  //////////////////////////////////////////////////////////
  ////////////////   specific field value   ////////////////
  //////////////////////////////////////////////////////////

  ParseOutput ParseFieldContentType(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    {
      ParseOutput parsed_main_type = ConsumeByParserFunction(&input, &ParseToken);
      if (parsed_main_type.status != kParseSuccess)
        return output;
    }
    if (ConsumeByCharacter(&input, '/') == 0)
    {
      temporary::arena.rollback(snapshot);
      return output;
    }
    {
      ParseOutput parsed_sub_type = ConsumeByParserFunction(&input, &ParseToken);
      if (parsed_sub_type.status != kParseSuccess)
      {
        temporary::arena.rollback(snapshot);
        return output;
      }
    }
    temporary::arena.rollback(snapshot);
    const char* content_end = input.bytes;
    PTNodeParameters* parameters = NULL;
    {
      ParseOutput parsed_parameters = ConsumeByParserFunction(&input, &ParseParameters);
      if (parsed_parameters.status == kParseSuccess)
        parameters = static_cast<PTNodeParameters*>(parsed_parameters.result);
      else
        return output;
    }

    PTNodeFieldContentType*  content_type = PTNodeCreate<PTNodeFieldContentType>();
    content_type->type = kFieldContentType;
    content_type->content = StringSlice(input_start, content_end - input_start);
    content_type->parameters = parameters;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = content_type;
    return output;
  }

  ParseOutput ParseFieldContentLength(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    int number = 0;

    while ((input.length > 0) && IsDigit(*input.bytes))
    {
      number = number * 10 + (*input.bytes - '0');
      input.consume();
    }
    if ((input.bytes - input_start) > 0)
    {
      PTNodeFieldContentLength*  content_length = PTNodeCreate<PTNodeFieldContentLength>();
      content_length->type = kFieldContentLength;
      content_length->number = number;

      output.status = kParseSuccess;
      output.parsed_length = input.bytes - input_start;
      output.result = content_length;
    }
    return output;
  }

  ParseOutput ParseFieldConnection(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    temporary::vector<PTNodeToken*> options;

    {
      ParseOutput parsed_connection_option = ConsumeByParserFunction(&input, &ParseToken);
      if (parsed_connection_option.status == kParseSuccess)
        options.push_back(static_cast<PTNodeToken*>(parsed_connection_option.result));
      else
      {
        while (input.length > 0)
        {
          Input input_tmp = input;
          if (!ConsumeByScanFunction(&input_tmp, &ScanOptionalWhitespace).is_valid() ||
              (ConsumeByCharacter(&input_tmp, ',') == 0) ||
              !ConsumeByScanFunction(&input_tmp, &ScanOptionalWhitespace).is_valid())
            break;
          parsed_connection_option = ConsumeByParserFunction(&input_tmp, &ParseToken);
          if (parsed_connection_option.status == kParseSuccess)
          {
            options.push_back(static_cast<PTNodeToken*>(parsed_connection_option.result));
            input = input_tmp;
          }
          else
            break;
        }
      }
    }
    PTNodeFieldConnection*  connection = PTNodeCreate<PTNodeFieldConnection>();
    connection->type = kFieldConnection;
    connection->options = options;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = connection;
    return output;
  }

  ParseOutput ParseFieldHost(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNodeUriHost*  uri_host;
    PTNodeUriPort*  port;

    {
      ParseOutput parsed_uri_host = ConsumeByParserFunction(&input, &ParseUriHost);
      if (parsed_uri_host.status == kParseSuccess)
        uri_host = static_cast<PTNodeUriHost*>(parsed_uri_host.result);
      else
        return output;
    }
    Input input_tmp = input;
    if (ConsumeByCharacter(&input_tmp, ':'))
    {
      ParseOutput parsed_port = ConsumeByParserFunction(&input_tmp, &ParseUriPort);
      if (parsed_port.status == kParseSuccess)
      {
        port = static_cast<PTNodeUriPort*>(parsed_port.result);
        input = input_tmp;
      }
    }

    PTNodeFieldHost*  host = PTNodeCreate<PTNodeFieldHost>();
    host->type = kFieldHost;
    host->host = uri_host;
    host->port = port;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = host;
    return output;
  }

  ParseOutput ParseFieldReferer(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    PTNode* uri_header = NULL;
    {
      ParseOutput parsed_uri_header = ConsumeByParserFunction(&input, &ParseUriAbsolute);
      if (parsed_uri_header.status == kParseSuccess)
        uri_header = parsed_uri_header.result;
      else
      {
        parsed_uri_header = ConsumeByParserFunction(&input, &ParseUriReferenceNetworkPath);
        if (parsed_uri_header.status == kParseSuccess)
          uri_header = parsed_uri_header.result;
        else
        {
          parsed_uri_header = ConsumeByParserFunction(&input, &ParsePathAbsolute);
          if (parsed_uri_header.status == kParseSuccess)
            uri_header = parsed_uri_header.result;
          else
          {
            parsed_uri_header = ConsumeByParserFunction(&input, &ParsePathNoScheme);
            if (parsed_uri_header.status == kParseSuccess)
              uri_header = parsed_uri_header.result;
            else
            {
              parsed_uri_header = ConsumeByParserFunction(&input, &ParsePathEmpty);
              if (parsed_uri_header.status == kParseSuccess)
                uri_header = parsed_uri_header.result;
              else
                return output;
            }
          }
        }
      }
    }

    PTNodeFieldReferer*  referer = PTNodeCreate<PTNodeFieldReferer>();
    referer->type = kFieldReferer;
    referer->uri_header = uri_header;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = referer;
    return output;
  }

  StringSlice ScanTransferCoding(Input input)
  {
    ScanOutput  scan_output;

    scan_output.length = input.match("chunked");
    if (scan_output.length != 7)
    {
      scan_output.length = input.match("compress");
      if (scan_output.length != 8)
      {
        scan_output.length = input.match("deflate");
        if (scan_output.length != 7)
        {
          scan_output.length = input.match("gzip");
          if (scan_output.length != 4)
          {
            scan_output.length = input.match("identity");
            if (scan_output.length != 8)
            {
              scan_output.length = input.match("trailers");
              if (scan_output.length != 8)
              {
                scan_output.length = 0;
                return scan_output;
              }
            }
          }
        }
      }
    }
    scan_output.bytes = input.bytes;
    return scan_output;
  }

  ParseOutput ParseFieldTransferEncoding(Input input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    temporary::vector<StringSlice> codings;

    StringSlice coding = ConsumeByScanFunction(&input, &ScanTransferCoding);
    if (coding.is_valid())
    {
      codings.push_back(coding);
      while (input.length > 0)
      {
        Input input_tmp = input;
        ConsumeByScanFunction(&input_tmp, &ScanOptionalWhitespace);
        if (ConsumeByCharacter(&input_tmp, ',') == 0)
          break;
        ConsumeByScanFunction(&input_tmp, &ScanOptionalWhitespace);
        coding = ConsumeByScanFunction(&input_tmp, &ScanTransferCoding);
        if (!coding.is_valid())
          break;
        else
          codings.push_back(coding);
      }
    }
    PTNodeFieldTransferEncoding*  transfer_encoding = PTNodeCreate<PTNodeFieldTransferEncoding>();
    transfer_encoding->type = kFieldTransferEncoding;
    transfer_encoding->codings = codings;

    output.status = kParseSuccess;
    output.parsed_length = input.bytes - input_start;
    output.result = transfer_encoding;
    return output;
  }

} // namespace http_parser

