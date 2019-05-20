#pragma once
#include <string>

struct Item
{
	void reset();
	std::string item_name = "";
	std::string description = "";
	int weight = 0;
};