#pragma once

#include <string>
#include <vector>

/*
	A header file which contains agreements on data handling
	regarding the HTTP protocol */

enum Method
{
	kGet,
	kPost,
	kDelete,
	kUnmatched
};

enum Version
{
	kStandard,
	kWrong
};
