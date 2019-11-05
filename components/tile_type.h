#pragma once
#include <string>

struct Tile_type
{
	void reset() {}
	std::string name;
	std::string description;
	bool accessible;
	bool transparent;
};