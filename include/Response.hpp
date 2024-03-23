#pragma once

#include "HTTPMessage.hpp"

class Response : public HTTPMessage
{
	public:
		Response();
		// + constructor with parameters?
		Response(Response const &obj);
		~Response();

		Response &operator=(Response const *obj);

	private:

};