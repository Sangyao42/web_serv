#include "LocationQuery.hpp"

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
      error_page(),
      access_log(),
      error_log() {}

  void  LocationQuery::construct(const directive::LocationBlock* location_block)
  {
    match_path = location_block->match();
    LocationQueryBuilder  builder;
    const directive::DirectiveBlock* block = location_block;

    while (block != NULL && !builder.all_filled())
    {
      std::vector<std::pair<Directive::Type, ConstructionFunc> >::iterator funcs_it = builder.one_off_funcs.begin();
      for (directive::Directives::const_iterator it = block->directives().begin();
          it != location_block->directives().end();
          ++it)
      {
        const Directive* directive = it->second;
        while (funcs_it != builder.one_off_funcs.end())
        {
          const Directive::Type& type = funcs_it->first;
          if (directive->type() != type)
          {
            const ConstructionFunc& func = funcs_it->second;
            func(*this, directive);
            funcs_it = builder.one_off_funcs.erase(funcs_it);
          }
          else
            ++funcs_it;
        }
      }
      block = block->parent();
    }
  }

  /////////////////////////////////////////////////////////
  ////////////   accumulatative construction   ////////////
  /////////////////////////////////////////////////////////

  void  ConstructCgis(LocationQuery& cache, const directive::LocationBlock* location_block)
  {
    (void) cache;
    (void) location_block;
  }

  void  ConstructIndexes(LocationQuery& cache, const directive::LocationBlock* location_block)
  {
    (void) cache;
    (void) location_block;
  }

  void  ConstructErrorPage(LocationQuery& cache, const directive::LocationBlock* location_block)
  {
    (void) cache;
    (void) location_block;
  }

  ///////////////////////////////////////////////
  ////////////   one off functions   ////////////
  ///////////////////////////////////////////////

  void  ConstructAllowMethods(LocationQuery& cache, const directive::AllowMethods* directive)
  {
    (void) cache;
    (void) directive;
  }

  void  ConstructClientMaxBodySize(LocationQuery& cache, const directive::ClientMaxBodySize* directive)
  {
    (void) cache;
    (void) directive;
  }

  void  ConstructRedirect(LocationQuery& cache, const directive::Redirect* directive)
  {
    (void) cache;
    (void) directive;
  }

  void  ConstructRoot(LocationQuery& cache, const directive::Root* directive)
  {
    (void) cache;
    (void) directive;
  }

  void  ConstructAutoindex(LocationQuery& cache, const directive::Autoindex* directive)
  {
    (void) cache;
    (void) directive;
  }

  void  ConstructMimeTypes(LocationQuery& cache, const directive::MimeTypes* directive)
  {
    (void) cache;
    (void) directive;
  }

  void  ConstructAccessLog(LocationQuery& cache, const directive::AccessLog* directive)
  {
    (void) cache;
    (void) directive;
  }

  void  ConstructErrorLog(LocationQuery& cache, const directive::ErrorLog* directive)
  {
    (void) cache;
    (void) directive;
  }

  //////////////////////////////////////////////////////
  ////////////   LocationQueryBuilder   ////////////
  //////////////////////////////////////////////////////

  LocationQueryBuilder::LocationQueryBuilder()
    : one_off_funcs(), accumulative_funcs()
  {
    one_off_funcs.reserve(8);
    one_off_funcs.push_back(std::make_pair(Directive::kDirectiveAllowMethods, reinterpret_cast<ConstructionFunc>(&ConstructAllowMethods)));
    one_off_funcs.push_back(std::make_pair(Directive::kDirectiveClientMaxBodySize, reinterpret_cast<ConstructionFunc>(&ConstructClientMaxBodySize)));
    one_off_funcs.push_back(std::make_pair(Directive::kDirectiveRedirect, reinterpret_cast<ConstructionFunc>(&ConstructRedirect)));
    one_off_funcs.push_back(std::make_pair(Directive::kDirectiveRoot, reinterpret_cast<ConstructionFunc>(&ConstructRoot)));
    one_off_funcs.push_back(std::make_pair(Directive::kDirectiveAutoindex, reinterpret_cast<ConstructionFunc>(&ConstructAutoindex)));
    one_off_funcs.push_back(std::make_pair(Directive::kDirectiveMimeTypes, reinterpret_cast<ConstructionFunc>(&ConstructMimeTypes)));
    one_off_funcs.push_back(std::make_pair(Directive::kDirectiveAccessLog, reinterpret_cast<ConstructionFunc>(&ConstructAccessLog)));
    one_off_funcs.push_back(std::make_pair(Directive::kDirectiveErrorLog, reinterpret_cast<ConstructionFunc>(&ConstructErrorLog)));

    accumulative_funcs.reserve(3);
    accumulative_funcs.push_back(std::make_pair(Directive::kDirectiveCgi, reinterpret_cast<ConstructionFunc>(&ConstructCgis)));
    accumulative_funcs.push_back(std::make_pair(Directive::kDirectiveIndex, reinterpret_cast<ConstructionFunc>(&ConstructIndexes)));
    accumulative_funcs.push_back(std::make_pair(Directive::kDirectiveErrorPage, reinterpret_cast<ConstructionFunc>(&ConstructErrorPage)));
  }

  bool  LocationQueryBuilder::all_filled() const
  {
    return one_off_funcs.empty();
  }

} // namespace cache
