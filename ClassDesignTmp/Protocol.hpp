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

enum StatusCode
{
	k000, // default value
	k200,
	k201,
	k204,
	k301,
	k304,
	k307,
	k400,
	k403,
	k404,
	k405,
	k406,
	k408,
	k412,
	k413,
	k414,
	k415,
	k422,
	k500,
	k503,
	// ...
	kError
};
