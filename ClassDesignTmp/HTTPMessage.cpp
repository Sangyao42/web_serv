#include "HTTPMessage.hpp"

HTTPMessage::HTTPMessage() {}

HTTPMessage::HTTPMessage(const HTTPMessage &obj)
{
	HeaderMapIt it;

	for (it = obj.headers_.begin(); it != obj.headers_.end(); ++it)
	{
		addNewPair(it->first, it->second->clone());
	}
}

HTTPMessage::~HTTPMessage()
{
	cleanHeaderMap();
}

HTTPMessage &HTTPMessage::operator=(const HTTPMessage &obj)
{
	cleanHeaderMap();

	HeaderMapIt it;

	for (it = obj.headers_.begin(); it != obj.headers_.end(); ++it)
	{
		addNewPair(it->first, it->second->clone());
	}
}

void	HTTPMessage::addNewPair(std::string key, HeaderValue *value)
{
	headers_.insert(std::make_pair(key, value));
}

void	HTTPMessage::addNewPair(HeaderPair pair)
{
	headers_.insert(pair);
}

HeaderValue	*HTTPMessage::returnValueAsPointer(std::string key) const
{
	HeaderMapIt it;

	for (it = headers_.begin(); it != headers_.end(); ++it)
	{
		if (it->first == key)
			return (it->second);
	}
	return (NULL); // key not found
}

HeaderValue	*HTTPMessage::returnValueAsClonedPointer(std::string key) const
{
	HeaderMapIt it;

	for (it = headers_.begin(); it != headers_.end(); ++it)
	{
		if (it->first == key)
			return (it->second->clone());
	}
	return (NULL); // key not found
}

const std::string	&HTTPMessage::returnMapAsString(std::string key) const
{
	// for generation of headers in response

	HeaderMapIt it;
	std::string	map;

	for (it = headers_.begin(); it != headers_.end(); ++it)
	{
		map += it->first + ": " + it->second->valueAsString() + "\r\n";
	}
	map += "\r\n";
	return (map);
}

HeaderPair	HTTPMessage::returnClonedPair(std::string key) const
{
	HeaderMapIt it;

	for (it = headers_.begin(); it != headers_.end(); ++it)
	{
		if (it->first == key)
			return (std::make_pair(key, it->second->clone()));
	}
	return (HeaderPair()); // key not found
}

void	HTTPMessage::cleanHeaderMap()
{
	HeaderMapIt it;

	for (it = headers_.begin(); it != headers_.end(); ++it)
		delete (it->second);
	headers_.clear();
}
