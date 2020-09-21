#pragma once
#include <string>

struct Shared_context;

struct Tile_type
{
	void reset() {}
	void load (const std::string& key, Shared_context* context) {}

	std::string name;
	std::string description;
	bool accessible;
	bool transparent;
};