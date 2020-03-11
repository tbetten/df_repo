#pragma once

#include <string>
#include <vector>

//#include "directions.h"
enum class Compass : int;
namespace tile_info
{
	struct Tile_info
	{
		std::string name;
		std::string description;
		bool accessible;
		bool transparant;
		bool ignited;
		Compass direction;
		int brightness;
	};

	using Tile_data = std::vector<Tile_info>;

	Tile_data load_tile_data(std::string path);
}