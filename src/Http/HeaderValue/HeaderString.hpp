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

		const std::string &content() const;
		std::string valueAsString();

	private:
		ValueType	type_;
		std::string content_;
};
