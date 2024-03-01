# HTTP parsers

## Fields

Field names are case-insensitive and ought to be registered within the "Hypertext Transfer Protocol (HTTP) Field Name Registry"; see Section 16.3.1.

Unless specified otherwise, fields are defined for all versions of HTTP. In particular, the [Host]((https://datatracker.ietf.org/doc/html/rfc9110#field.host)) and [Connection](https://datatracker.ietf.org/doc/html/rfc9110#field.connection) fields ought to be recognized by all HTTP implementations whether or not they advertise conformance with HTTP/1.1.

A proxy MUST forward unrecognized header fields unless the field name is listed in the Connection header field (Section 7.6.1) or the proxy is specifically configured to block, or otherwise transform, such fields. Other recipients SHOULD ignore unrecognized header and trailer fields. Adhering to these requirements allows HTTP's functionality to be extended without updating or removing deployed intermediaries.

### Field names

When a field name is repeated within a section, its combined field value consists of the list of corresponding field line values within that section, concatenated in order, with each field line value separated by a comma.

The order in which field lines with differing field names are received in a section is not significant. However, it is good practice to send header fields that contain additional control data first, such as Host on requests and Date on responses, so that implementations can decide when not to handle a message as early as possible.

A server MUST NOT apply a request to the target resource until it receives the entire request header section, since later header field lines might include conditionals, authentication credentials, or deliberately misleading duplicate header fields that could impact request processing.

A server that receives a request header field line, field value, or set of fields larger than it wishes to process MUST respond with an appropriate 4xx (Client Error) status code. Ignoring such header fields would increase the server's vulnerability to request smuggling attacks (Section 11.2 of [HTTP/1.1]).

### Field values

Field values are usually constrained to the range of [US-ASCII characters](http://www.columbia.edu/kermit/ascii.html).

a recipient of CR, LF, or NUL within a field value MUST either reject the message or replace each of those characters with SP before further processing or forwarding of that message.

Field values containing other CTL characters are also invalid; however, recipients MAY retain such characters for the sake of robustness when they appear within a safe context (e.g., an application-specific quoted string that will not be processed by any downstream HTTP parser).

singleton fields - only anticipate a single member as the field value
list-based fields - allow multiple members as the field value. They use comma as delimiter.

#### List-based field values

For example, a textual date and a URI (either of which might contain a comma) could be safely carried in list-based field values like these:

```
Example-URIs: "http://example.com/a.html,foo", "http://without-a-comma.example.com/"
Example-Dates: "Sat, 04 May 1996", "Wed, 14 Sep 2005"
```

Note that double-quote delimiters are almost always used with the quoted-string production (Section 5.6.4); using a different syntax inside double-quotes will likely cause unnecessary confusion.

Many fields (such as Content-Type, defined in Section 8.3) use a common syntax for parameters that allows both unquoted (token) and quoted (quoted-string) syntax for a parameter value (Section 5.6.6). Use of common syntax allows recipients to reuse existing parser components. When allowing both forms, the meaning of a parameter value ought to be the same whether it was received as a token or a quoted string.

This is fine:

```
"foo,bar"
"foo ,bar,"
"foo , ,bar,charlie"
```

But not this:

```
""
","
",   ,"
```

### References

- [HTTP message fields](https://datatracker.ietf.org/doc/html/rfc9110#fields)
- [HTTP Headers registry](https://www.iana.org/assignments/http-fields/http-fields.xhtml)
- [RFC 9110 HTTP Semantics](https://datatracker.ietf.org/doc/html/rfc9110)
- [URI Syntax](https://datatracker.ietf.org/doc/html/rfc3986)
- [ABNF specification](https://datatracker.ietf.org/doc/html/rfc5234#autoid-24)
