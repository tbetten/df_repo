#pragma once
#include <vector>
#include <optional>
#include "ecs_types.h"

struct Tileset
{
	std::string source;
	unsigned int firstgid;
	std::optional<unsigned int> lastgid;
};

struct Map
{
	void reset() {}
	std::vector<Tileset> tilesets;
	std::vector<ecs::Entity_id> tiles;
	unsigned int width;
	unsigned int height;
	unsigned int tilewidth;
	unsigned int tileheight;
};