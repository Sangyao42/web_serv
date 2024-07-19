#pragma once

#include <string>
#include <vector>

/*
	A header file which contains agreements on data handling
	regarding the HTTP protocol */

enum Method
{
	kGet = 1,
	kPost = 2,
	kDelete = 4,
	kUnmatched = 8, // 501 Not Implemented
	kWrong = 16 // 400 Bad Request
};

enum Version
{
	kStandard, // HTTP/1.1
	kCompatible, // HTTP/0.9, 1.0, 2.0 (it remains to be HTTP/1.1 compliant),
	kBad // 400 Bad Request
};

enum StatusCode
{
	k000 = 0, // default value
	k200 = 200,
	k201 = 201,
	k204 = 204,
	k301 = 301,
	k303 = 303,
	k304 = 304,
	k307 = 307,
	k400 = 400,
	k403 = 403,
	k404 = 404,
	k405 = 405,
	k406 = 406,
	k408 = 408,
	k411 = 411,
	k412 = 412,
	k413 = 413,
	k414 = 414,
	k415 = 415,
	k422 = 422,
	k500 = 500,
	k501 = 501,
	k503 = 503,
	k504 = 504,
  	k505 = 505,
	kError = 999
};
