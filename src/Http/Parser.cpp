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
    : status(kParseFailure), rest(), parsed_length(0), result(NULL) {}

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
    output.rest = input;
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
        output.rest = input;
        output.parsed_length = content.length;
        output.result = token;
      }
    }
    return output;
  }

  ParseOutput  ParseQuotedString(Input input)
  {
    ParseOutput output;
    output.rest = input;
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
          output.rest = input;
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
    output.rest = input;
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
          output.rest = input;
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
    output.rest = input;

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
          output.rest = input;
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
    output.rest = input;
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
      output.rest = input;
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
    }
    return output;
  }

  ParseOutput  ParsePathAbEmpty(Input input)
  {
    ParseOutput output;
    output.rest = input;
    return output;
  }

  ParseOutput  ParsePathAbsolute(Input input)
  {
    ParseOutput output;
    output.rest = input;
    return output;
  }

  ParseOutput  ParsePathNoScheme(Input input)
  {
    ParseOutput output;
    output.rest = input;
    return output;
  }

  ParseOutput  ParsePathRootless(Input input)
  {
    ParseOutput output;
    output.rest = input;
    return output;
  }

  ParseOutput  ParsePathEmpty(Input input)
  {
    ParseOutput output;
    output.status = kParseSuccess;
    output.rest = input;
    output.parsed_length = 0;
    output.result = static_cast<PTNode*>(PTNodeCreate<PTNodePathEmpty>());
    output.result->type = kPathEmpty;
    return output;
  }

} // namespace http_parser

