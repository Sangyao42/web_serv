#include "LocationQuery.hpp"

#include <cassert>

#include <string>
#include <vector>

#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Block.hpp"
#include "Configuration/Directive/Block/Location.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Simple.hpp"
#include "Configuration/Directive/Simple/MimeTypes.hpp"
#include "Configuration/Directive/Simple/AllowMethods.hpp"
#include "Configuration/Directive/Simple/Redirect.hpp"
#include "Configuration/Directive/Simple/Cgi.hpp"

namespace cache
{

  ////////////////////////////////////////////////
  ////////////   LocationQueryCache   ////////////
  ////////////////////////////////////////////////

  LocationQuery::LocationQuery()
    : match_path(),
      allowed_methods(),
      client_max_body_size(0),
      redirect(NULL),
      cgis(),
      root(),
      indexes(),
      autoindex(false),
      mime_types(),
      error_pages(),
      access_log(),
      error_log() {}

  void  LocationQuery::construct(const directive::LocationBlock* location_block)
  {
    construct_match_path(location_block);
    construct_allowed_methods(location_block);
    construct_client_max_body_size(location_block);
    construct_redirect(location_block);
    construct_cgis(location_block);
    construct_root(location_block);
    construct_indexes(location_block);
    construct_autoindex(location_block);
    construct_mime_types(location_block);
    construct_error_pages(location_block);
    construct_access_log(location_block);
    construct_error_log(location_block);
  }

  void  LocationQuery::construct_match_path(const directive::LocationBlock* location_block)
  {
    match_path = location_block->match();
  }

  void  LocationQuery::construct_allowed_methods(const directive::LocationBlock* location_block)
  {
    const directive::AllowMethods* directive = 
      static_cast<const directive::AllowMethods*>(closest_directive(location_block, Directive::kDirectiveAllowMethods));

    allowed_methods = directive ? directive->get() : directive::kMethodGet | directive::kMethodPost;
  }

  void  LocationQuery::construct_client_max_body_size(const directive::LocationBlock* location_block)
  {
    const directive::ClientMaxBodySize* directive = 
      static_cast<const directive::ClientMaxBodySize*>(closest_directive(location_block, Directive::kDirectiveClientMaxBodySize));
 
    client_max_body_size = directive ? directive->get() : 1048576; // 1MB
  }

  void  LocationQuery::construct_redirect(const directive::LocationBlock* location_block)
  {
    redirect = static_cast<const directive::Redirect*>(closest_directive(location_block, Directive::kDirectiveRedirect));
  }

  void  LocationQuery::construct_cgis(const directive::LocationBlock* location_block)
  {
    std::vector<const Directive*> cgis_directives = collect_directives(location_block, Directive::kDirectiveCgi, &CheckCgi);
    cgis.reserve(cgis_directives.size());
    for (std::vector<const Directive*>::const_iterator it = cgis_directives.begin(); it != cgis_directives.end(); ++it)
    {
      cgis.push_back(static_cast<const directive::Cgi*>(*it));
    }
  }

  void  LocationQuery::construct_root(const directive::LocationBlock* location_block)
  {
    const directive::Root* directive = 
      static_cast<const directive::Root*>(closest_directive(location_block, Directive::kDirectiveRoot));

    root = directive ? directive->get() : "";
  }

  void  LocationQuery::construct_indexes(const directive::LocationBlock* location_block)
  {
    std::vector<const Directive*> indexes_directives = collect_directives(location_block, Directive::kDirectiveIndex, &CheckIndex);
    indexes.reserve(indexes_directives.size());
    for (std::vector<const Directive*>::const_iterator it = indexes_directives.begin(); it != indexes_directives.end(); ++it)
    {
      indexes.push_back(static_cast<const directive::Index*>(*it));
    }
  }

  void  LocationQuery::construct_autoindex(const directive::LocationBlock* location_block)
  {
    const directive::Autoindex* directive = 
      static_cast<const directive::Autoindex*>(closest_directive(location_block, Directive::kDirectiveAutoindex));
    
    autoindex = directive ? directive->get() : false;
  }

  void  LocationQuery::construct_mime_types(const directive::LocationBlock* location_block)
  {
    mime_types = static_cast<const directive::MimeTypes*>(closest_directive(location_block, Directive::kDirectiveMimeTypes));
  }

  void  LocationQuery::construct_error_pages(const directive::LocationBlock* location_block)
  {
    std::vector<const Directive*> error_pages_directives = collect_directives(location_block, Directive::kDirectiveErrorPage, &CheckErrorPage);
    error_pages.reserve(error_pages_directives.size());
    for (std::vector<const Directive*>::const_iterator it = error_pages_directives.begin(); it != error_pages_directives.end(); ++it)
    {
      error_pages.push_back(static_cast<const directive::ErrorPage*>(*it));
    }
  }

  void  LocationQuery::construct_access_log(const directive::LocationBlock* location_block)
  {
    const directive::AccessLog* directive = 
      static_cast<const directive::AccessLog*>(closest_directive(location_block, Directive::kDirectiveAccessLog));

    access_log = directive ? directive->get() : "";
  }

  void  LocationQuery::construct_error_log(const directive::LocationBlock* location_block)
  {
    const directive::ErrorLog* directive = 
      static_cast<const directive::ErrorLog*>(closest_directive(location_block, Directive::kDirectiveErrorLog));

    error_log = directive ? directive->get() : "";
  }

  const Directive*  LocationQuery::closest_directive(const directive::DirectiveBlock* block, Directive::Type type)
  {
    assert(block != NULL);
    int index = block->index();
    while (block)
    {
      directive::DirectivesRange query_result = block->query_directive(type);
      if (directive::DirectiveRangeIsValid(query_result))
      {
        // Find the last directive with index less than the current block
        for (directive::DirectivesRange::first_type it = query_result.second; it != query_result.first; --it)
        {
          const Directive* directive = it->second;
          if (directive->index() < index)
          {
            return directive;
          }
        }
      }
      index = block->index();
      block = block->parent();
    }
    return NULL;
  }

  std::vector<const Directive*>  LocationQuery::collect_directives(const directive::DirectiveBlock* block,
                                                                   Directive::Type type,
                                                                   DuplicateChecker is_duplicated)
  {
    std::vector<const Directive*> result;
    int index = block->index();
    while (block)
    {
      directive::DirectivesRange query_result = block->query_directive(type);
      if (directive::DirectiveRangeIsValid(query_result))
      {
        for (directive::DirectivesRange::second_type it = query_result.second; it != query_result.first; --it)
        {
          const Directive* directive = it->second;
          if (directive->index() < index)
          {
            // If a directive is duplicated, only the first one is kept, so do nothing
            std::vector<const Directive*>::iterator result_it = result.begin();
            for (; result_it != result.end(); ++result_it)
            {
              if (is_duplicated(result_it, directive))
                break;
            }
            // otherwise, add it to the result
            if (result_it == result.end())
              result.push_back(directive);
          }
        }
      }
      index = block->index();
      block = block->parent();
    }
    return result;
  }

  bool  CheckCgi(std::vector<const Directive*>::iterator directive_it, const Directive* new_directive)
  {
    const directive::Cgi* directive = static_cast<const directive::Cgi*>(new_directive);
    return static_cast<const directive::Cgi*>(*directive_it)->get() == directive->get();
  }

  bool  CheckIndex(std::vector<const Directive*>::iterator directive_it, const Directive* new_directive)
  {
    const directive::Index* directive = static_cast<const directive::Index*>(new_directive);
    return static_cast<const directive::Index*>(*directive_it)->get() == directive->get();
  }

  bool  CheckErrorPage(std::vector<const Directive*>::iterator directive_it, const Directive* new_directive)
  {
    const directive::ErrorPage* directive = static_cast<const directive::ErrorPage*>(new_directive);
    return static_cast<const directive::ErrorPage*>(*directive_it)->get() == directive->get();
  }
} // namespace caches
