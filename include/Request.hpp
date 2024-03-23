#pragma once

#include "HTTPMessage.hpp"

class Request : public HTTPMessage
{
	public:
		Request();
		// + constructor with parameters
		Request(Request const &obj);
		~Request();

		Request &operator=(Request const &obj);

	private:

};