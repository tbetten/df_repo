#pragma once
#include <string>
#include "ecs_types.h"
#include "position.h"
#include "drawable.h"
#include "tilemap.h"
#include <SFML/System.hpp>
#include <type_traits>
#include <unordered_map>

struct Shared_context;
struct Map_data;

namespace el
{
	enum class Tile_type { Invalid = -1, Inert_tile, Dynamic_tile, Item };

	struct Array_data
	{
		Vertex_array array;
		std::string tilesheet;
		std::string map;
		std::string layer;
	};

	class Entity_loader
	{
	public:
		Entity_loader(Shared_context* context) : m_context{ context } {}
		void load_map(const std::string& handle);
		bool set_position(ecs::Entity_id entity, sf::Vector2i coords, Position::Layer layer, std::string map);
		bool set_player_controlled(ecs::Entity_id entity, bool player_controlled);
	private:
		ecs::Entity_id create_entity(tiled::Tilemap& map, tiled::Tile& tile, Tile_type type, const std::string& map_name, Map_data* md);
		void create_inert_tile(tiled::Tilemap& map, tiled::Tile& tile, const std::string& map_name);
		ecs::Entity_id create_dynamic_tile(tiled::Tilemap& map, tiled::Tile& tile, const std::string& map_name, Map_data* md, bool position = true, bool drawable = true);
		ecs::Entity_id create_item(tiled::Tilemap& map, tiled::Tile& tile, const std::string& map_name, Map_data* md);
		void load_item_properties(ecs::Entity_id entity, const std::string& handle);
		void finalize_inert_tiles(Map_data* md);

		Shared_context* m_context;
		std::unordered_map<std::string, Array_data> m_arrays;
	};
}