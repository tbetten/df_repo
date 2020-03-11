#pragma once
#include <string>
#include <vector>
#include <memory>
#include "resource_cache.h"

struct Shared_context;

namespace tileset
{
	struct Animation_frame
	{
		int tile_id = 0;
		int duration = 0;
	};

	using Tile_type = std::pair<int, std::string>;
	using Tile_types = std::vector<Tile_type>;
	using Animation = std::vector<Animation_frame>;
	using Animations = std::vector<Animation>;

	struct Tileset
	{
		Tileset() :name{ "" }, firstgid{ 0 }, lastgid{ 0 }, tilewidth{ 0 }, tileheight{ 0 }, texture{ nullptr }, columns{ 0 }, source{ "" } {}
		Tileset(std::string source_, int firstgid_ = 0) : source{ std::move(source_) }, firstgid{ firstgid_ }, name{ "" }, lastgid{ 0 }, tilewidth{ 0 }, tileheight{ 0 }, texture{ nullptr }, columns{ 0 }{}
		sf::IntRect get_rect(int tile_id);
		std::string name;
		int firstgid;
		int lastgid;
		int tilewidth;
		int tileheight;
		std::shared_ptr<cache::Resource_base> texture;
		int columns;
		std::string source;
		Tile_types tile_types;
		Animations animations;
	};

	void load_tileset(std::string_view filename, Tileset& tileset, Shared_context* context);
}