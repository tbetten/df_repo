#pragma once

#include <string>
#include <vector>

namespace tile_info
{
	struct Tile_info
	{
		std::string name;
		std::string description;
		bool accessible;
		bool transparant;
	};

	using Tile_data = std::vector<Tile_info>;

	Tile_data load_tile_data(std::string path);
}