#pragma once

#include "HeaderValue.hpp"

class HeaderString : public HeaderValue
{
	public:
		HeaderString();
		HeaderString(const std::string &content);
		HeaderString(const HeaderString &obj);
		~HeaderString();

		HeaderString &operator=(const HeaderString &obj);

		ValueType	type() const;
		std::string valueAsString();

		std::string& content();

	private:
		std::string content_;
};
