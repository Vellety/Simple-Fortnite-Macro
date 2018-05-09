#pragma once

// Pleb security LUL
// Probably very unnecessary, but you never know *shrug*

#include <vector>
#include <random>
#include <functional>

typedef std::vector<char> char_array;

char_array charset()
{
	// Should be good enough
	return char_array(
		{ '0','1','2','3','4',
		'5','6','7','8','9',
		'A','B','C','D','E','F',
		'G','H','I','J','K',
		'L','M','N','O','P',
		'Q','R','S','T','U',
		'V','W','X','Y','Z',
		'a','b','c','d','e','f',
		'g','h','i','j','k',
		'l','m','n','o','p',
		'q','r','s','t','u',
		'v','w','x','y','z'
		});
};

// Generates random character and returns a string of the requested length
std::string random_string(size_t length, std::function<char(void)> rand_char)
{
	std::string str(length, 0);
	std::generate_n(str.begin(), length, rand_char);
	return str;
}