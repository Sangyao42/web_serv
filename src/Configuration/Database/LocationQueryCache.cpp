#include "LocationQueryCache.hpp"

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

////////////////////////////////////////////////
////////////   LocationQueryCache   ////////////
////////////////////////////////////////////////

LocationQueryCache::LocationQueryCache()
  : match_path(),
    allowed_methods(),
    client_max_body_size(0),
    redirect(NULL),
    cgis(),
    root(),
    indexes(),
    autoindex(false),
    mime_types(),
    error_page(),
    access_log(),
    error_log() {}

void  LocationQueryCache::construct(const directive::LocationBlock* location_block)
{
  construct_match_path(location_block);
  LocationQueryCacheFilled  filled;
  const directive::DirectiveBlock* block = location_block;

  while (block != NULL && !filled.all_filled())
  {
    std::vector<std::pair<Directive::Type, ConstructionFunc> >::iterator funcs_it = filled.construction_funcs.begin();
    for (directive::Directives::const_iterator it = block->directives().begin();
        it != location_block->directives().end();
        ++it)
    {
      const Directive* directive = it->second;
      while (funcs_it != filled.construction_funcs.end())
      {
        const Directive::Type& type = funcs_it->first;
        if (directive->type() != type)
        {
          const ConstructionFunc& func = funcs_it->second;
          func(*this, directive);
          funcs_it = filled.construction_funcs.erase(funcs_it);
        }
        else
          ++funcs_it;
      }
    }
    block = block->parent();
  }
  construct_cgis(location_block);
  construct_indexes(location_block);
  construct_error_page(location_block);
}

void  LocationQueryCache::construct_match_path(const directive::LocationBlock* location_block)
{
  match_path = location_block->match();
}

void  LocationQueryCache::construct_cgis(const directive::LocationBlock* location_block)
{
  (void) location_block;
}

void  LocationQueryCache::construct_indexes(const directive::LocationBlock* location_block)
{
  (void) location_block;
}

void  LocationQueryCache::construct_error_page(const directive::LocationBlock* location_block)
{
  (void) location_block;
}

////////////////////////////////////////////////////
////////////   construction functions   ////////////
////////////////////////////////////////////////////

void  ConstructAllowMethods(LocationQueryCache& cache, const directive::AllowMethods* directive)
{
  (void) cache;
  (void) directive;
}

void  ConstructClientMaxBodySize(LocationQueryCache& cache, const directive::ClientMaxBodySize* directive)
{
  (void) cache;
  (void) directive;
}

void  ConstructRedirect(LocationQueryCache& cache, const directive::Redirect* directive)
{
  (void) cache;
  (void) directive;
}

void  ConstructRoot(LocationQueryCache& cache, const directive::Root* directive)
{
  (void) cache;
  (void) directive;
}

void  ConstructAutoindex(LocationQueryCache& cache, const directive::Autoindex* directive)
{
  (void) cache;
  (void) directive;
}

void  ConstructMimeTypes(LocationQueryCache& cache, const directive::MimeTypes* directive)
{
  (void) cache;
  (void) directive;
}

void  ConstructAccessLog(LocationQueryCache& cache, const directive::AccessLog* directive)
{
  (void) cache;
  (void) directive;
}

void  ConstructErrorLog(LocationQueryCache& cache, const directive::ErrorLog* directive)
{
  (void) cache;
  (void) directive;
}

//////////////////////////////////////////////////////
////////////   LocationQueryCacheFilled   ////////////
//////////////////////////////////////////////////////

LocationQueryCacheFilled::LocationQueryCacheFilled()
  : construction_funcs()
{
  construction_funcs.reserve(8);
  construction_funcs.push_back(std::make_pair(Directive::kDirectiveAccessLog, reinterpret_cast<ConstructionFunc>(&ConstructAllowMethods)));
  construction_funcs.push_back(std::make_pair(Directive::kDirectiveAccessLog, reinterpret_cast<ConstructionFunc>(&ConstructClientMaxBodySize)));
  construction_funcs.push_back(std::make_pair(Directive::kDirectiveAccessLog, reinterpret_cast<ConstructionFunc>(&ConstructRedirect)));
  // construction_funcs.push_back(reinterpret_cast<ConstructionFunc>(&ConstructCgis));
  construction_funcs.push_back(std::make_pair(Directive::kDirectiveAccessLog, reinterpret_cast<ConstructionFunc>(&ConstructRoot)));
  // construction_funcs.push_back(reinterpret_cast<ConstructionFunc>(&ConstructIndexes));
  construction_funcs.push_back(std::make_pair(Directive::kDirectiveAccessLog, reinterpret_cast<ConstructionFunc>(&ConstructAutoindex)));
  construction_funcs.push_back(std::make_pair(Directive::kDirectiveAccessLog, reinterpret_cast<ConstructionFunc>(&ConstructMimeTypes)));
  // construction_funcs.push_back(reinterpret_cast<ConstructionFunc>(&ConstructErrorPage));
  construction_funcs.push_back(std::make_pair(Directive::kDirectiveAccessLog, reinterpret_cast<ConstructionFunc>(&ConstructAccessLog)));
  construction_funcs.push_back(std::make_pair(Directive::kDirectiveAccessLog, reinterpret_cast<ConstructionFunc>(&ConstructErrorLog)));
}

bool  LocationQueryCacheFilled::all_filled() const
{
  return construction_funcs.empty();
}
