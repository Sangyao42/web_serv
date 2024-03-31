#include "Main.hpp"

#include "Configuration/Directive/Block.hpp"

namespace directive
{
  MainBlock::MainBlock()
    : DirectiveBlock(),
      http_(),
      events_()
  {
    http_.set_parent(this);
    events_.set_parent(this);
  }

  MainBlock::MainBlock(const Context& context)
    : DirectiveBlock(context),
      http_(),
      events_()
  {
    http_.set_parent(this);
    events_.set_parent(this);
  }

  MainBlock::MainBlock(const MainBlock& other)
    : DirectiveBlock(other),
      http_(other.http_),
      events_(other.events_)
  {
    http_.set_parent(this);
    events_.set_parent(this);
  }

  MainBlock& MainBlock::operator=(const MainBlock& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      http_ = other.http_;
      events_ = other.events_;
      http_.set_parent(this);
      events_.set_parent(this);
    }
    return *this;
  }

  MainBlock::~MainBlock() {}

  DirectiveBlock::Type  MainBlock::type() const
  {
    return Directive::kDirectiveMain;
  }

  HttpBlock&  MainBlock::http()
  {
    return http_;
  }

  const HttpBlock&  MainBlock::http() const
  {
    return http_;
  }

  EventsBlock&  MainBlock::events()
  {
    return events_;
  }

  const EventsBlock&  MainBlock::events() const
  {
    return events_;
  }
} // namespace directive
