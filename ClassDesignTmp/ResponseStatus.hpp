#pragma once

#include <string>

// Range 100 .. 599

/***************************
 * INFORMATIONAL RESPONSES *
 ***************************/

// #define STATUS_100 "100 Continue"
// #define STATUS_101 "101 Switching Protocols"
// #define STATUS_102 "102 Processing"
// #define STATUS_103 "103 Early Hints"

/************************
 * SUCCESSFUL RESPONSES *
 ************************/

#define STATUS_200 "200 OK"
#define STATUS_201 "201 Created" // for POST requests, when the request created a new resource 
// #define STATUS_202 "202 Accepted"
// #define STATUS_203 "203 Non-Authoritative Information"
#define STATUS_204 "204 No Content" // useful header information only, successful delete operation
#define STATUS_205 "205 Reset Content"

enum StatusCode
{
	k000, // default value
	k200,
	k201,
	k204,
	k205,
	kError
};

std::string	statusCodeAsString(enum StatusCode code)
{
	switch (code)
	{
		case k200:
			return ("200 OK");
		case k201:
			return ("201 Created");
		case k204:
			return ("204 No Content");
		case k205:
			return ("205 Reset Content");
		default:
		 return (std::string()); // Error
	}	
}