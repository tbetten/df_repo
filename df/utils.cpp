#include "utils.h"

#include <algorithm>
#include <cctype>

bool iequal(std::string_view s1, std::string_view s2)
{
	if (s1.size() != s2.size()) return false;
	auto itr = std::mismatch(std::cbegin(s1), std::cend(s1), std::cbegin(s2), [](char c1, char c2) {return std::tolower(c1) == std::tolower(c2); });
	return itr.first == std::cend(s1) ? true : false;
}

std::string to_lower(std::string s)
{
	std::transform(std::begin(s), std::end(s), std::begin(s), [](unsigned char s) {return std::tolower(s); });
	return s;
}