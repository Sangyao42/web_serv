#pragma once

#include <map>
#include "HeaderValue/HeaderValue.hpp"

typedef std::pair<std::string, HeaderValue *>	HeaderPair;
typedef std::map<std::string, HeaderValue *>	HeaderMap;
typedef HeaderMap::const_iterator HeaderMapIt;

/*
	An abstract class having member variables and functions
	shared by Request and Response classes */

class HTTPMessage
{
	public:
		HTTPMessage();
		HTTPMessage(const HTTPMessage &obj);
		virtual ~HTTPMessage();

		HTTPMessage &operator=(const HTTPMessage &obj);

		std::map<std::string, HeaderValue *>	headers_;

		void	addNewPair(std::string key, HeaderValue *value);
		void	addNewPair(HeaderPair pair);

		HeaderValue	*returnValueAsPointer(std::string key) const; // memory managed by this class
		HeaderValue	*returnValueAsClonedPointer(std::string key) const; // should be freed elsewhere
		HeaderPair	returnClonedPair(std::string key) const; // should be freed elsewhere
		std::string	returnMapAsString() const;

	protected:
		void	cleanHeaderMap();
};
