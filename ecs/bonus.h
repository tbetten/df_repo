#pragma once
#include <string>

struct Bonus
{
	enum class Type { Direct, Percent };
	int amount;
	Type type;
	bool timed;
	int remaining_time;
	std::string message;
};