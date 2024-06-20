#pragma once

#include "HeaderValue.hpp"

class HeaderInt : public HeaderValue
{
	public:
		HeaderInt();
		HeaderInt(const int &content);
		HeaderInt(const HeaderInt &obj);
		~HeaderInt();

		HeaderInt &operator=(const HeaderInt &obj);

		const ValueType	&type() const;
		const int &content() const;
		HeaderInt	*clone() const;
    std::string to_string() const;

	private:
		ValueType	type_;
		int content_;
};
