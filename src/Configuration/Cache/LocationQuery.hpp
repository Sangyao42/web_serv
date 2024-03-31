#pragma once

#include <string>
#include <vector>

#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Block/Location.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Simple.hpp"
#include "Configuration/Directive/Simple/MimeTypes.hpp"
#include "Configuration/Directive/Simple/AllowMethods.hpp"
#include "Configuration/Directive/Simple/Redirect.hpp"
#include "Configuration/Directive/Simple/Cgi.hpp"

namespace cache
{
   
  /**
   * @brief Properties of a location block
  */
  struct LocationQuery
  {
    std::string                   match_path;
    // direvtives to decide if the request is allowed
    directive::Methods            allowed_methods;
    size_t                        client_max_body_size;
    // If redirect is not null, the request should be generated a redirect response
    directive::Redirect*          redirect;
    // If cgi is not null, the request should be handled by a cgi script
    std::vector<directive::Cgi*>  cgis;
    // directives to find the full path of the requested resource
    std::string                   root;
    std::vector<std::string>      indexes;
    bool                          autoindex;
    const directive::MimeTypes*   mime_types;
    std::string                   error_page;
    std::string                   access_log;
    std::string                   error_log;

    LocationQuery();

    void  construct(const directive::LocationBlock* location_block);

    private:
      void  construct_match_path(const directive::LocationBlock* location_block);
      void  construct_cgis(const directive::LocationBlock* location_block);
      void  construct_indexes(const directive::LocationBlock* location_block);
      void  construct_error_page(const directive::LocationBlock* location_block);
  };

  void  ConstructAllowMethods(LocationQuery& cache, const directive::AllowMethods* directive);
  void  ConstructClientMaxBodySize(LocationQuery& cache, const directive::ClientMaxBodySize* directive);
  void  ConstructRedirect(LocationQuery& cache, const directive::Redirect* directive);
  void  ConstructRoot(LocationQuery& cache, const directive::Root* directive);
  void  ConstructAutoindex(LocationQuery& cache, const directive::Autoindex* directive);
  void  ConstructMimeTypes(LocationQuery& cache, const directive::MimeTypes* directive);
  void  ConstructAccessLog(LocationQuery& cache, const directive::AccessLog* directive);
  void  ConstructErrorLog(LocationQuery& cache, const directive::ErrorLog* directive);

  typedef void  (*ConstructionFunc)(LocationQuery&, const Directive*);

  struct LocationQueryCacheFilled
  {
    std::vector<std::pair<Directive::Type, ConstructionFunc> >  construction_funcs;

    LocationQueryCacheFilled();
    bool  all_filled() const;
  };
} // namespace caches
