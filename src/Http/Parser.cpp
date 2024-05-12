#include "Parser.hpp"

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
    return ((character >= 0 && character <= 31) || (character == 127));
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
    return (character >= 128 && character <= 255);
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

} // namespace http_parser

