#pragma once

#include "HeaderValue.hpp"
#include <vector>

typedef std::vector<std::string> StringVector;
typedef std::vector<std::string>::iterator StringVectorIt;

class HeaderStringVector : public HeaderValue
{
	public:
		HeaderStringVector();
		HeaderStringVector(const StringVector &content);
		HeaderStringVector(const HeaderStringVector &obj);
		~HeaderStringVector();

		HeaderStringVector &operator=(const HeaderStringVector &obj);

		const ValueType	&type() const;
		const StringVector &content() const;
		HeaderStringVector	*clone() const;
		void	toStringStream(std::ostringstream& oss);

		void	addString(const std::string &str);
		void	addString(const char *str);
		void	addString(const char *str, size_t len);

		size_t	size() const;

		const std::string &operator[](size_t index) const;

	private:
		ValueType	type_;
		StringVector content_;
};
