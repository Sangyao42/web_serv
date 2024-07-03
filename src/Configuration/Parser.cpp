#include "Parser.hpp"

#include <cstdlib>
#include "Configuration/Directive/Block/Events.hpp"
#include "Configuration/Directive/Block/Http.hpp"
#include "Configuration/Directive/Block/Location.hpp"
#include "Configuration/Directive/Block/Main.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Http/Parser.hpp"

namespace directive_parser
{
  ScanOutput  ScanOptionalBlank(ParseInput input)
  {
    ScanOutput output;

    output.bytes = input.bytes;
    while (input.length > 0)
    {
      const char* target = input.consume();
      if (target && !(http_parser::IsSpace(*target) || http_parser::IsHorizontalTab(*target) || http_parser::IsLinefeed(*target)))
        break;
      output.length++;
    }
    return output;
  }

  ParseOutput ParseMainBlock(ParseInput input)
  {
    ParseOutput parse_output;
    const char* input_start = input.bytes;
    directive::MainBlock* main_block = new directive::MainBlock();
    http_parser::ConsumeByScanFunction(&input, ScanOptionalBlank);
    {
      while (input.length > 0)
      {
        ParseInput  input_temp = input;
        Directive*  directive = NULL;
        if (http_parser::ConsumeByCString(&input_temp, "http") == 4)
        {
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          ParseOutput parsed_http_block = http_parser::ConsumeByParserFunction(&input_temp, &ParseHttpBlock);
          if (parsed_http_block.is_valid())
          {
            directive = static_cast<Directive*>(parsed_http_block.result);
          }
        }
        else if (http_parser::ConsumeByCString(&input_temp, "events") == 6)
        {
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          ParseOutput parsed_events_block = http_parser::ConsumeByParserFunction(&input_temp, &ParseEventsBlock);
          if (parsed_events_block.is_valid())
          {
            directive = static_cast<Directive*>(parsed_events_block.result);
          }
        }
        else if (http_parser::ConsumeByCString(&input_temp, "error_log") == 9)
        {
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          ParseOutput parsed_error_log = http_parser::ConsumeByParserFunction(&input_temp, &ParseErrorLog);
          if (parsed_error_log.is_valid())
          {
            directive = static_cast<Directive*>(parsed_error_log.result);
          }
        }

        if (directive)
        {
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          http_parser::ConsumeByCString(&input_temp, ";");
          http_parser::ConsumeByScanFunction(&input_temp, &ScanOptionalBlank);
          main_block->add_directive(directive);
          input = input_temp;
        }
        else
        {
          delete main_block;
          return parse_output;
        }
      }
    }
    parse_output.result = main_block;
    parse_output.length = input.bytes - input_start;
    return parse_output;
  }

  ParseOutput ParseEventsBlock(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if (http_parser::ConsumeByCString(&input, "{") == 1)
    {
      directive::EventsBlock* event_block = new directive::EventsBlock();
      bool  is_valid = true;
      while ((input.length > 0) && is_valid)
      {
        http_parser::ConsumeByScanFunction(&input, &ScanOptionalBlank);
        ParseInput  input_temp = input;
        Directive*  directive = NULL;
        if (http_parser::ConsumeByCString(&input_temp, "worker_connection") == 17)
        {
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          ParseOutput parsed_worker_connection = http_parser::ConsumeByParserFunction(&input_temp, &ParseWorkerConnections);
          if (parsed_worker_connection.is_valid())
          {
            http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
            http_parser::ConsumeByCString(&input_temp, ";");
            input = input_temp;
            event_block->add_directive(static_cast<Directive*>(parsed_worker_connection.result));
          }
          else
          {
            delete event_block;
            break;
          }
        }
      }
      if (is_valid && (http_parser::ConsumeByCString(&input, "}") == 1))
      {
        output.result = event_block;
        output.length = input.bytes - input_start;
      }
    }
    return output;
  }

  ParseOutput ParseCommonContent(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    Directive*  directive = NULL;

    if (http_parser::ConsumeByCString(&input, "allow_methods") == 13)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseAllowMethods);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    else if (http_parser::ConsumeByCString(&input, "root") == 4)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseRoot);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    else if (http_parser::ConsumeByCString(&input, "index") == 5)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseIndex);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    else if (http_parser::ConsumeByCString(&input, "types") == 5)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseMimeTypes);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    else if (http_parser::ConsumeByCString(&input, "error_pages") == 11)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseErrorPages);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    else if (http_parser::ConsumeByCString(&input, "client_max_body_size") == 20)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseClientMaxBodySize);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    else if (http_parser::ConsumeByCString(&input, "autoindex") == 9)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseAutoIndex);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    else if (http_parser::ConsumeByCString(&input, "cgi") == 3)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseCgi);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    else if (http_parser::ConsumeByCString(&input, "access_log") == 10)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseAccessLog);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    else if (http_parser::ConsumeByCString(&input, "error_log") == 9)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseErrorLog);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    else if (http_parser::ConsumeByCString(&input, "return") == 6)
    {
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
      ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input, &ParseReturn);
      if (parsed_directive.is_valid())
      {
        directive = static_cast<Directive*>(parsed_directive.result);
      }
    }
    if (directive)
    {
      output.result = directive;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseHttpBlock(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if (http_parser::ConsumeByCString(&input, "{") == 1)
    {
      directive::HttpBlock* http_block = new directive::HttpBlock();
      while (input.length > 0)
      {
        http_parser::ConsumeByScanFunction(&input, &ScanOptionalBlank);
        ParseInput  input_temp = input;
        Directive*  directive = NULL;
        if (http_parser::ConsumeByCString(&input_temp, "server") == 6)
        {
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          ParseOutput parsed_server_block = http_parser::ConsumeByParserFunction(&input_temp, &ParseServerBlock);
          if (parsed_server_block.is_valid())
          {
            directive = static_cast<Directive*>(parsed_server_block.result);
          }
        }
        else
        {
          ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input_temp, &ParseCommonContent);
          if (parsed_directive.is_valid())
          {
            directive = static_cast<Directive*>(parsed_directive.result);
          }
        }
        if (directive)
        {
          http_block->add_directive(directive);
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          http_parser::ConsumeByCString(&input_temp, ";");
          input = input_temp;
        }
        else
        {
          delete http_block;
          http_block = NULL;
          break;
        }
      }
      if (http_block && (http_parser::ConsumeByCString(&input, "}") == 1))
      {
        output.result = http_block;
        output.length = input.bytes - input_start;
      }
    }
    return output;
  }

  ParseOutput ParseServerBlock(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if (http_parser::ConsumeByCString(&input, "{") == 1)
    {
      directive::ServerBlock* server_block = new directive::ServerBlock();
      while (input.length > 0)
      {
        http_parser::ConsumeByScanFunction(&input, &ScanOptionalBlank);
        ParseInput  input_temp = input;
        Directive*  directive = NULL;
        if (http_parser::ConsumeByCString(&input_temp, "location") == 8)
        {
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          ParseOutput parsed_location_block = http_parser::ConsumeByParserFunction(&input_temp, &ParseLocationBlock);
          if (parsed_location_block.is_valid())
          {
            directive = static_cast<Directive*>(parsed_location_block.result);
          }
        }
        else if (http_parser::ConsumeByCString(&input_temp, "listen") == 6)
        {
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          ParseOutput parsed_listen = http_parser::ConsumeByParserFunction(&input_temp, &ParseListen);
          if (parsed_listen.is_valid())
          {
            directive = static_cast<Directive*>(parsed_listen.result);
          }
        }
        else if (http_parser::ConsumeByCString(&input_temp, "server_name") == 11)
        {
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          ParseOutput parsed_server_name = http_parser::ConsumeByParserFunction(&input_temp, &ParseServerName);
          if (parsed_server_name.is_valid())
          {
            directive = static_cast<Directive*>(parsed_server_name.result);
          }
        }
        else
        {
          ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input_temp, &ParseCommonContent);
          if (parsed_directive.is_valid())
          {
            directive = static_cast<Directive*>(parsed_directive.result);
          }
        }
        if (directive)
        {
          server_block->add_directive(directive);
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          http_parser::ConsumeByCString(&input_temp, ";");
          input = input_temp;
        }
        else
        {
          delete server_block;
          server_block = NULL;
          break;
        }
      }
      if (server_block && (http_parser::ConsumeByCString(&input, "}") == 1))
      {
        output.result = server_block;
        output.length = input.bytes - input_start;
      }
    }
    return output;
  }

  ParseOutput ParseLocationBlock(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if (http_parser::ConsumeByCString(&input, "{") == 1)
    {
      directive::LocationBlock* location_block = new directive::LocationBlock();
      while (input.length > 0)
      {
        http_parser::ConsumeByScanFunction(&input, &ScanOptionalBlank);
        ParseInput  input_temp = input;
        Directive*  directive = NULL;
        if (http_parser::ConsumeByCString(&input_temp, "location") == 8)
        {
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          ParseOutput parsed_location_block = http_parser::ConsumeByParserFunction(&input_temp, &ParseLocationBlock);
          if (parsed_location_block.is_valid())
          {
            directive = static_cast<Directive*>(parsed_location_block.result);
          }
        }
        else
        {
          ParseOutput parsed_directive = http_parser::ConsumeByParserFunction(&input_temp, &ParseCommonContent);
          if (parsed_directive.is_valid())
          {
            directive = static_cast<Directive*>(parsed_directive.result);
          }
        }
        if (directive)
        {
          location_block->add_directive(directive);
          http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
          http_parser::ConsumeByCString(&input_temp, ";");
          input = input_temp;
        }
        else
        {
          delete location_block;
          location_block = NULL;
          break;
        }
      }
      if (location_block && (http_parser::ConsumeByCString(&input, "}") == 1))
      {
        output.result = location_block;
        output.length = input.bytes - input_start;
      }
    }
    return output;
  }

  ParseOutput ParseRoot(ParseInput input)
  {

  }

  ParseOutput ParseIndex(ParseInput input)
  {

  }

  ParseOutput ParseAutoIndex(ParseInput input)
  {

  }

  ParseOutput ParseClientMaxBodySize(ParseInput input)
  {

  }

  ParseOutput ParseAccessLog(ParseInput input)
  {

  }

  ParseOutput ParseErrorLog(ParseInput input)
  {

  }

  ParseOutput ParseWorkerConnections(ParseInput input)
  {

  }


  ParseOutput ParseAllowMethods(ParseInput input)
  {

  }

  ParseOutput ParseCgi(ParseInput input)
  {

  }

  ParseOutput ParseErrorPages(ParseInput input)
  {

  }

  ParseOutput ParseListen(ParseInput input)
  {

  }

  ParseOutput ParseMimeTypes(ParseInput input)
  {

  }

  ParseOutput ParseReturn(ParseInput input)
  {

  }

  ParseOutput ParseServerName(ParseInput input)
  {
  
  }

} // namespace directive_parser

