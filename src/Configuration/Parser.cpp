#include "Parser.hpp"

#include <cstdlib>
#include "Arenas.hpp"
#include "Http/Parser.hpp"
#include "Configuration/Directive/Block/Events.hpp"
#include "Configuration/Directive/Block/Http.hpp"
#include "Configuration/Directive/Block/Location.hpp"
#include "Configuration/Directive/Block/Main.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Simple.hpp"
#include "Configuration/Directive/Simple/AllowMethods.hpp"
#include "Configuration/Directive/Simple/Cgi.hpp"
#include "Configuration/Directive/Simple/ErrorPage.hpp"
#include "Configuration/Directive/Simple/Listen.hpp"
#include "Configuration/Directive/Simple/MimeTypes.hpp"
#include "Configuration/Directive/Simple/Return.hpp"
#include "Configuration/Directive/Simple/ServerName.hpp"

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
      while (input.length > 0)
      {
        http_parser::ConsumeByScanFunction(&input, &ScanOptionalBlank);
        ParseInput  input_temp = input;
        if (http_parser::ConsumeByCString(&input, "}") == 1)
        {
          output.result = event_block;
          output.length = input.bytes - input_start;
          return output;
        }
        else if (http_parser::ConsumeByCString(&input_temp, "worker_connection") == 17)
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
        if (http_parser::ConsumeByCString(&input, "}") == 1)
        {
          output.result = http_block;
          output.length = input.bytes - input_start;
          return output;
        }
        else if (http_parser::ConsumeByCString(&input_temp, "server") == 6)
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
          break;
        }
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
        if (http_parser::ConsumeByCString(&input, "}") == 1)
        {
          output.result = server_block;
          output.length = input.bytes - input_start;
          return output;
        }
        else if (http_parser::ConsumeByCString(&input_temp, "location") == 8)
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
          break;
        }
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
      location_block->set(((http_parser::PTNodePathAbsolute*)parsed_path.result)->content.to_string());
      temporary::arena.rollback(snapshot);
      http_parser::ConsumeByScanFunction(&input, &ScanOptionalBlank);
      if (http_parser::ConsumeByCString(&input, "{") == 1)
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
      }
    }
    return output;
  }

  // Windows (FAT32, NTFS): Any Unicode except NUL, \, /, :, *, ?, ", <, >, |. Also, no space character at the start or end, and no period at the end.
  // Mac(HFS, HFS+): Any valid Unicode except : or /
  // Linux(ext[2-4]): Any byte except NUL or /
  // so any byte except NUL, \, /, :, *, ?, ", <, >, | and you can't have files/folders call . or .. and no control characters (of course).

  bool  IsOSFileCharacter(char c)
  {
#ifdef __WIN32
    return ((std::strchr("\\/:*?\"<>|", c) == NULL) && (c != 0));
#elif defined __APPLE__
    return (std::strchr(":/", c) == NULL);
#elif defined __unix__
    return ((c != '/') && (c != 0));
#endif
  }

  ScanOutput  ScanOSFileName(ParseInput input)
  {
    ScanOutput output;

    output.bytes = input.bytes;
    while (input.length > 0)
    {
      const char* target = input.consume();
      if (target)
      {
        if (http_parser::IsSpace(*target) || http_parser::IsHorizontalTab(*target) || !IsOSFileCharacter(*target))
          break;
      }
      output.length++;
    }
    if (output.length < 1)
      output.bytes = NULL;
    return output;
  }

  ScanOutput  ScanOSPath(ParseInput input)
  {
    ScanOutput output;
    bool  last_is_slash = false;

    output.bytes = input.bytes;
    if (input.length > 0)
    {
      if (*input.bytes == '\\')
      {
        last_is_slash = true;
        input.consume();
      }
    }
    while (input.length > 0)
    {
      if (last_is_slash)
      {
        ScanOutput tmp = ScanOSFileName(input);
        if (tmp.is_valid())
        {
          input.consume(tmp.length);
        }
        else
          break;
      }
      else
      {
        if (*input.bytes == '\\')
        {
          last_is_slash = true;
          input.consume();
        }
        else
          break;
      }
      output.length++;
    }
    if (output.length < 1)
      output.bytes = NULL;
    return output;
  }

  ParseOutput ParseRoot(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ScanOutput path = http_parser::ConsumeByScanFunction(&input, &ScanOSPath);
    if (path.is_valid())
    {
      directive::Root*  root = new directive::Root();
      root->set(path.to_string());
      output.result = root;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseIndex(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ScanOutput path = http_parser::ConsumeByScanFunction(&input, &ScanOSPath);
    if (path.is_valid())
    {
      directive::Index*  index = new directive::Index();
      index->set(path.to_string());
      output.result = index;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseAutoIndex(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if (http_parser::ConsumeByCString(&input, "on") == 2)
    {
      directive::Autoindex* autoindex = new directive::Autoindex();
      autoindex->set(true);
      output.result = autoindex;
      output.length = input.bytes - input_start;
    }
    else if (http_parser::ConsumeByCString(&input, "off") == 3)
    {
      directive::Autoindex* autoindex = new directive::Autoindex();
      autoindex->set(false);
      output.result = autoindex;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseClientMaxBodySize(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    size_t number = 0;

    // TODO: can overflow
    while ((input.length > 0) && http_parser::IsDigit(*input.bytes))
    {
      number = number * 10 + (*input.bytes - '0');
      input.consume();
    }
    if ((input.bytes - input_start) > 0)
    {
      if (http_parser::ConsumeByCString(&input, "k"))
      {
        number *= 1000;
      }
      else if (http_parser::ConsumeByCString(&input, "m"))
      {
        number *= 1000000;
      }
      directive::ClientMaxBodySize* client_max_body_size = new directive::ClientMaxBodySize();
      client_max_body_size->set(number);
      output.result = client_max_body_size;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseAccessLog(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ScanOutput path = http_parser::ConsumeByScanFunction(&input, &ScanOSPath);
    if (path.is_valid())
    {
      directive::AccessLog*  access_log = new directive::AccessLog();
      access_log->set(path.to_string());
      output.result = access_log;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseErrorLog(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ScanOutput path = http_parser::ConsumeByScanFunction(&input, &ScanOSPath);
    if (path.is_valid())
    {
      directive::ErrorLog*  error_log = new directive::ErrorLog();
      error_log->set(path.to_string());
      output.result = error_log;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseWorkerConnections(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    int number = 0;
    while ((input.length > 0) && http_parser::IsDigit(*input.bytes))
    {
      number = number * 10 + (*input.bytes - '0');
      input.consume();
    }
    if (((input.bytes - input_start) > 0) && (number > 0))
    {
      directive::WorkerConnections* worker_connections = new directive::WorkerConnections();
      worker_connections->set(number);
      output.result = worker_connections;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseAllowMethods(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    directive::Methods methods = 0;

    while (input.length > 0)
    {
      if (http_parser::ConsumeByCString(&input, "GET"))
      {
        methods |= directive::kMethodGet;
      }
      else if (http_parser::ConsumeByCString(&input, "POST"))
      {
        methods |= directive::kMethodPost;
      }
      else if (http_parser::ConsumeByCString(&input, "DELETE"))
      {
        methods |= directive::kMethodDelete;
      }
      http_parser::ConsumeByScanFunction(&input, &http_parser::ScanOptionalWhitespace);
    }
    if (methods == 0)
    {
      directive::AllowMethods*  allow_methods = new directive::AllowMethods();
      allow_methods->set(methods);
      output.result = allow_methods;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseCgi(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ScanOutput  extension = http_parser::ConsumeByScanFunction(&input, &ScanOSFileName);
    if (extension.is_valid())
    {
      if (http_parser::ConsumeByScanFunction(&input, &http_parser::ScanRequiredWhitespace).is_valid())
      {
        ScanOutput path = http_parser::ConsumeByScanFunction(&input, &ScanOSPath);
        if (path.is_valid())
        {
          directive::Cgi* cgi = new directive::Cgi();
          cgi->set(extension.to_string(), path.to_string());
          output.result = cgi;
          output.length = input.bytes - input_start;
        }
      }
    }
    return output;
  }

  ParseOutput ParseStatusCode(ParseInput input)
  {
    ParseOutput output;
    static int  status_code = 0;
    const char* input_start = input.bytes;

    status_code = -1;
    int number = 0;
    while ((input.length > 0) && http_parser::IsDigit(*input.bytes))
    {
      number = number * 10 + (*input.bytes - '0');
      input.consume();
    }
    if ((number >= 100) && (number <= 511))
    {
      status_code = number;
      output.result = &status_code;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseErrorPages(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ParseOutput parsed_status_code = http_parser::ConsumeByParserFunction(&input, &ParseStatusCode);
    if (parsed_status_code.is_valid())
    {
      if (http_parser::ConsumeByScanFunction(&input, &http_parser::ScanRequiredWhitespace).is_valid())
      {
        ScanOutput path = http_parser::ConsumeByScanFunction(&input, &ScanOSPath);
        if (path.is_valid())
        {
          directive::ErrorPage*  error_page = new directive::ErrorPage();
          error_page->set(*((int *)parsed_status_code.result), path.to_string());
          output.result = error_page;
          output.length = input.bytes - input_start;
        }
      }
    }
    return output;
  }

  ParseOutput ParseListen(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    directive::Listen* listen = new directive::Listen();
    uri::Authority authority;

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    ParseOutput tmp = http_parser::ConsumeByParserFunction(&input, &http_parser::ParseUriAuthority);
    if (tmp.is_valid() && (AnalysisUriAuthority((http_parser::PTNodeUriAuthority*) tmp.result, &authority) == kNone))
    {
      listen->add(authority);
      temporary::arena.rollback(snapshot);
    }
    else
    {
      delete listen;
      return output;
    }
    while (input.length > 0)
    {
      if (!http_parser::ConsumeByScanFunction(&input, &http_parser::ScanRequiredWhitespace).is_valid())
        break;
      snapshot = temporary::arena.snapshot();
      ParseOutput tmp = http_parser::ConsumeByParserFunction(&input, &http_parser::ParseUriAuthority);
      if (tmp.is_valid() && (AnalysisUriAuthority((http_parser::PTNodeUriAuthority*) tmp.result, &authority) == kNone))
      {
        listen->add(authority);
        temporary::arena.rollback(snapshot);
      }
      else
      {
        break;
      }
    }
    output.result = listen;
    output.length = input.bytes - input_start;
    return output;
  }

  ParseOutput ParseMimeTypes(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    if (http_parser::ConsumeByCString(&input, "{") == 1)
    {
      directive::MimeTypes* mime_types = new directive::MimeTypes();
      std::string extension, mime_type;
      while (input.length > 0)
      {
        http_parser::ConsumeByScanFunction(&input, &ScanOptionalBlank);
        ParseInput  input_temp = input;

        ArenaSnapshot snapshot = temporary::arena.snapshot();
        ParseOutput parsed_mime_type = http_parser::ConsumeByParserFunction(&input_temp, &http_parser::ParseFieldContentType);
        if (parsed_mime_type.is_valid())
        {
          mime_type = ((http_parser::PTNodeFieldContentType*)parsed_mime_type.result)->content.to_string();
          if (http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanRequiredWhitespace).is_valid())
          {
            ParseOutput parsed_extension = http_parser::ConsumeByParserFunction(&input_temp, &http_parser::ParseToken);
            if (parsed_extension.is_valid())
            {
              extension = ((http_parser::PTNodeToken*) parsed_extension.result)->content.to_string();
              mime_types->add(extension, mime_type);
              http_parser::ConsumeByScanFunction(&input_temp, &http_parser::ScanOptionalWhitespace);
              http_parser::ConsumeByCString(&input_temp, ";");
              input = input_temp;
            }
          }
          temporary::arena.rollback(snapshot);
        }
        delete mime_types;
        mime_types = NULL;
        break;
      }
      if (mime_types && (http_parser::ConsumeByCString(&input, "}") == 1))
      {
        output.result = mime_types;
        output.length = input.bytes - input_start;
      }
    }
    return output;
  }

  ParseOutput ParseReturn(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;

    ParseOutput parsed_status_code = http_parser::ConsumeByParserFunction(&input, &ParseStatusCode);
    if (parsed_status_code.is_valid())
    {
      directive::Return*  return_directive = new directive::Return();
      ParseInput input_tmp = input;
      std::string uri;
      if (http_parser::ConsumeByScanFunction(&input_tmp, &http_parser::ScanRequiredWhitespace).is_valid())
      {
        ArenaSnapshot snapshot = temporary::arena.snapshot();
        const char* uri_start = input_tmp.bytes;
        ParseOutput parsed_uri = http_parser::ConsumeByParserFunction(&input_tmp, &http_parser::ParseUri);
        if (parsed_uri.is_valid())
        {
          uri = std::string(uri_start, parsed_uri.length);
          input = input_tmp;
        }
        temporary::arena.rollback(snapshot);
      }
      return_directive->set(uri, *((int*) parsed_status_code.result));
      output.result = return_directive;
      output.length = input.bytes - input_start;
    }
    return output;
  }

  ParseOutput ParseServerName(ParseInput input)
  {
    ParseOutput output;
    const char* input_start = input.bytes;
    directive::ServerName* server_name = new directive::ServerName(); 

    ArenaSnapshot snapshot = temporary::arena.snapshot();
    ParseOutput parsed_regname = http_parser::ConsumeByParserFunction(&input, &http_parser::ParseRegName);
    if (parsed_regname.is_valid())
    {
      server_name->add(((http_parser::PTNodeRegName*)parsed_regname.result)->content.to_string());
      temporary::arena.rollback(snapshot);
    }
    else
    {
      delete server_name;
      return output;
    }
    while (input.length > 0)
    {
      if (!http_parser::ConsumeByScanFunction(&input, &http_parser::ScanRequiredWhitespace).is_valid())
        break;
      snapshot = temporary::arena.snapshot();
      ParseOutput parsed_regname = http_parser::ConsumeByParserFunction(&input, &http_parser::ParseRegName);
      if (parsed_regname.is_valid())
      {
        server_name->add(((http_parser::PTNodeRegName*)parsed_regname.result)->content.to_string());
        temporary::arena.rollback(snapshot);
      }
      else
        break;
    }
    output.result = server_name;
    output.length = input.bytes - input_start;
    return output;
  }

} // namespace directive_parser

