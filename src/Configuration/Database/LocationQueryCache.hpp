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

/**
 * @brief Properties of a location block
*/
struct LocationQueryCache
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

  LocationQueryCache();

  void  construct(const directive::LocationBlock* location_block);

  private:
    void  construct_match_path(const directive::LocationBlock* location_block);
    void  construct_cgis(const directive::LocationBlock* location_block);
    void  construct_indexes(const directive::LocationBlock* location_block);
    void  construct_error_page(const directive::LocationBlock* location_block);
};

void  ConstructAllowMethods(LocationQueryCache& cache, const directive::AllowMethods* directive);
void  ConstructClientMaxBodySize(LocationQueryCache& cache, const directive::ClientMaxBodySize* directive);
void  ConstructRedirect(LocationQueryCache& cache, const directive::Redirect* directive);
void  ConstructRoot(LocationQueryCache& cache, const directive::Root* directive);
void  ConstructAutoindex(LocationQueryCache& cache, const directive::Autoindex* directive);
void  ConstructMimeTypes(LocationQueryCache& cache, const directive::MimeTypes* directive);
void  ConstructAccessLog(LocationQueryCache& cache, const directive::AccessLog* directive);
void  ConstructErrorLog(LocationQueryCache& cache, const directive::ErrorLog* directive);

typedef void  (*ConstructionFunc)(LocationQueryCache&, const Directive*);

struct LocationQueryCacheFilled
{
  std::vector<std::pair<Directive::Type, ConstructionFunc> >  construction_funcs;

  LocationQueryCacheFilled();
  bool  all_filled() const;
};
