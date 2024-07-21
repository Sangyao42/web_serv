#pragma once

#include <string>

class Directive
{
  public:
    struct Context
    {
      int index;

      Context();
      Context(int index);

      bool  operator==(const Context& context) const;
    };

    enum Type
    {
      // Blocks
      kDirectiveMain,
      kDirectiveHttp,
      kDirectiveServer,
      kDirectiveEvents,
      kDirectiveLocation,
      // for HTTP requests
      kDirectiveListen,
      kDirectiveServerName,
      kDirectiveAllowMethods,
      // for HTTP response generation (serve a file)
      kDirectiveRoot,
      kDirectiveIndex,
      kDirectiveMimeTypes,
      kDirectiveErrorPage,
      // for HTTP request generation (generating content)
      kDirectiveClientMaxBodySize,
      kDirectiveReturn,
      kDirectiveAutoindex,
      kDirectiveCgi,
      // misc
      kDirectiveAccessLog,
      kDirectiveErrorLog,
      kDirectiveInclude,
      // only in events block
      kDirectiveWorkerConnections
    };
    Directive();
    explicit Directive(const Context& context);
    Directive(const Directive& other);
    Directive& operator=(const Directive& other);
    virtual ~Directive();

    virtual bool          is_block() const = 0;
    virtual Type          type() const = 0;
	virtual void		  print(int indentation) const = 0;
    const Context&        context() const;
    void                  set_context(const Context& context);
    int                   index() const;

  protected:
    Context context_;
};

void	PrintIndentation(int indentation);
