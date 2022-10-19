#pragma once
#include <string>
#include <base64.h>

class Base64Wrapper
{
public:
	/*
	* Both functions used to convert strings values to base64 value and vice versa. 
	*/
	static std::string encode(const std::string& str);
	static std::string decode(const std::string& str);
};
