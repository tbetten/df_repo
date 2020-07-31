#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <SFML/Graphics.hpp>
//#include "resource_cache.h"

struct Shared_context;

namespace cache
{
	class Resource_base;
}

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

	class Tileset
	{
	public:
		Tileset() : name{ "" }, source{ "" } {}
		//Tileset(std::string source_, int firstgid_ = 0) : source{ std::move(source_) }, firstgid{ firstgid_ }, name{ "" }{}
		void load(std::string_view filename);
		sf::IntRect get_rect(int tile_id) const;
		void set_first_gid(int first_gid) noexcept;
		std::tuple<int, int> get_gids() const noexcept; 

		sf::Texture texture;
		Tile_types tile_types;
		Animations animations;
	private:
		std::string name{ "" };
		int firstgid{ 0 };
		int lastgid{ 0 };
		int num_tiles{ 0 };
		int tilewidth{ 0 };
		int tileheight{ 0 };
		//std::shared_ptr<cache::Resource_base> texture;
		int columns{ 0 };
		std::string source{ "" };

	};

	struct Tile
	{
		sf::Sprite get_sprite();

		std::shared_ptr<cache::Resource_base> tileset_resource;
		std::shared_ptr<Tileset> tileset;
		int tile_index;
	};

	//load_tileset(std::string_view filename);
}