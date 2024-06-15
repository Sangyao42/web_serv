#include "HeaderStringVector.hpp"

HeaderStringVector::HeaderStringVector()
: type_(kStringVector), content_(StringVector()) {}

HeaderStringVector::HeaderStringVector(const StringVector &content)
: type_(kStringVector), content_(content) {}

HeaderStringVector::HeaderStringVector(const HeaderStringVector &obj)
: type_(obj.type_), content_(obj.content_) {}

HeaderStringVector::~HeaderStringVector()
{
	content_.clear();
}

HeaderStringVector &HeaderStringVector::operator=(const HeaderStringVector &obj)
{
	content_.clear();
	type_ = obj.type_;
	content_ = obj.content_;
}

const HeaderStringVector::ValueType	&HeaderStringVector::type() const
{
	return (type_);
}

const StringVector	&HeaderStringVector::content() const
{
	return (content_);
}

HeaderStringVector	*HeaderStringVector::clone() const
{
	return (new HeaderStringVector(*this));
}

void	HeaderStringVector::toStringStream(std::ostringstream &oss)
{
	for (StringVectorIt it = content_.begin(); it != content_.end(); ++it)
	{
		oss << *it;
		if (it + 1 != content_.end())
			oss << ", ";
	}
}

void	HeaderStringVector::addString(const std::string &str)
{
	content_.push_back(str);
}

void	HeaderStringVector::addString(const char *str)
{
	content_.push_back(std::string(str));
}

void	HeaderStringVector::addString(const char *str, size_t len)
{
	content_.push_back(std::string(str, len));
}

void	HeaderStringVector::removeString(size_t index)
{
	assert(index < content_.size() && "Index out of bounds");
	content_.erase(content_.begin() + index);
}

void	HeaderStringVector::removeString(StringVectorIt it)
{
	content_.erase(it);
}

void	HeaderStringVector::removeString(const std::string &str)
{
	StringVectorIt it = std::find(content_.begin(), content_.end(), str);
	if (it != content_.end())
		content_.erase(it);
}

size_t	HeaderStringVector::size() const
{
	return (content_.size());
}

const std::string	&HeaderStringVector::operator[](size_t index) const
{
	assert(index < content_.size() && "Index out of bounds");
	return (content_[index]);
}
