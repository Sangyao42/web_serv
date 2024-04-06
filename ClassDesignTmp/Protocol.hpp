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
	kUnmatched // 501 Not Implemented
};

enum Version
{
	kStandard, // HTTP/1.1
	kCompatible, // HTTP/0.9, 1.0, 2.0 (it remains to be HTTP/1.1 compliant),
	kBad // 400 Bad Request
};

