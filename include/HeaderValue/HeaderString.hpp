#pragma once

#include "HeaderValue.hpp"

class HeaderString : public HeaderValue
{
	public:
		HeaderString();
		HeaderString(std::string content);
		HeaderString(HeaderString const &obj);
		~HeaderString();

		HeaderString &operator=(HeaderString const &obj);

		std::string content() const;

	private:
		std::string content_;
};
