#pragma once

#include <map>
#include <iterator>
#include "Protocol.hpp"
#include "HeaderValue.hpp"

typedef std::map<std::string, HeaderValue *>::iterator HeaderMapIt;
typedef std::pair<std::string, HeaderValue *>	HeaderPair;

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

	protected:
		std::map<std::string, HeaderValue *>	headers_;

		void	addNewPair(std::string key, HeaderValue *value);
		void	addNewPair(HeaderPair pair);

		HeaderValue	*returnValueAsPointer(std::string key); // memory managed by this class
		HeaderValue	*returnValueAsClonePointer(std::string key); // should be freed elsewhere
		std::string	returnValueAsString(std::string key); // not implemented yet
		HeaderPair	returnClonedPair(std::string key); // should be freed elsewhere

		void	cleanHeaderMap();
};
