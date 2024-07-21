#include "./Block.hpp"

#include <cassert>
#include <iostream>

#include "Configuration/Directive.hpp"

namespace directive
{

  /////////////////////////////////////////////
  ////////////   Directive Block   ////////////
  /////////////////////////////////////////////

  DirectiveBlock::DirectiveBlock()
    : Directive(), directives_(), parent_(NULL) {}

  DirectiveBlock::DirectiveBlock(const Context& context)
    : Directive(context), directives_(), parent_(NULL) {}

  DirectiveBlock::DirectiveBlock(const DirectiveBlock& other)
    : Directive(other), directives_(other.directives_), parent_(NULL) {}

  DirectiveBlock& DirectiveBlock::operator=(const DirectiveBlock& other)
  {
    Directive::operator=(other);
    directives_ = other.directives_;
    return *this;
  }

  DirectiveBlock::~DirectiveBlock()
  {
    for (Directives::iterator it = directives_.begin(); it != directives_.end(); ++it)
    {
      delete it->second;
    }
  }

  bool DirectiveBlock::is_block() const
  {
    return true;
  }

  void DirectiveBlock::print(int indentation) const
  {
    for (Directives::const_iterator it = directives_.begin(); it != directives_.end(); it++)
    {
      PrintIndentation(indentation);
      const char* name = NULL;

      switch (it->first)
      {
      case kDirectiveMain: name = "main"; break;
      case kDirectiveHttp: name = "http"; break;
	  case kDirectiveServer: name = "server"; break;
	  case kDirectiveEvents: name = "events"; break;
	  case kDirectiveLocation: name = "location"; break;
	  case kDirectiveListen: name = "listen"; break;
	  case kDirectiveServerName: name = "server_name"; break;
	  case kDirectiveAllowMethods: name = "allow_methods"; break;
	  case kDirectiveRoot: name = "root"; break;
	  case kDirectiveIndex: name = "index"; break;
	  case kDirectiveMimeTypes: name = "mime_type"; break;
	  case kDirectiveErrorPage: name = "error_pages"; break;
	  case kDirectiveClientMaxBodySize: name = "client_max_body_size"; break;
	  case kDirectiveReturn: name = "return"; break;
	  case kDirectiveAutoindex: name = "autoindex"; break;
	  case kDirectiveCgi: name = "cgi"; break;
	  case kDirectiveAccessLog: name = "access_log"; break;
	  case kDirectiveErrorLog: name = "error_log"; break;
	  case kDirectiveInclude: name = "include"; break;
	  case kDirectiveWorkerConnections: name = "worker_connections"; break;
      }
	  std::cout << name << ": ";
	  if ((it->first == kDirectiveMain) ||
          (it->first == kDirectiveHttp) ||
          (it->first == kDirectiveServer) ||
          (it->first == kDirectiveEvents) ||
          (it->first == kDirectiveLocation))
      {
        std::cout << '{' << std::endl;
        it->second->print(indentation + 2);
		PrintIndentation(indentation);
        std::cout << '}' << std::endl;
	  }
	  else
      {
        it->second->print(indentation + 2);
        std::cout << std::endl;
      }
    }
  }

  const Directives& DirectiveBlock::directives() const
  {
    return directives_;
  }

  DirectiveBlock* DirectiveBlock::parent()
  {
    return parent_;
  }

  const DirectiveBlock* DirectiveBlock::parent() const
  {
    return parent_;
  }

  void DirectiveBlock::set_parent(DirectiveBlock* parent)
  {
    parent_ = parent;
  }

  void DirectiveBlock::add_directive(Directive* directive)
  {
    assert(directive != NULL);
    directive->set_context(directives_.size());
    if (directive->is_block())
    {
      static_cast<DirectiveBlock*>(directive)->set_parent(this);
    }
    directives_.insert(std::make_pair(directive->type(), directive));
  }

  DirectivesRange DirectiveBlock::query_directive(Type type) const
  {
    return directives_.equal_range(type);
  }

  Directives::const_iterator DirectiveBlock::end() const
  {
    return directives_.end();
  }

  bool DirectiveRangeIsValid(const DirectivesRange& range)
  {
    return (range != DirectivesRange()) && (range.first != range.second);
  }
} // namespace configuration
