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

		const ValueType	&type() const;

		const std::string &content() const;
		HeaderString	*clone() const;
		void	toStringStream(std::ostringstream& oss);

	private:
		ValueType	type_;
		std::string content_;
};
