#pragma once
#include <SFML/System/Vector2.hpp>
#include <unordered_map>
#include <vector>
#include <array>
#include <string>
#include <optional>

#include "ecs_types.h"


struct Map_data
{
	using cell_data = std::array<std::optional<ecs::Entity_id>, 10>;
	using layer_data = std::vector<cell_data>;
	enum class Orientation { Orthogonal, Hexagonal, Isometric };
	Orientation orientation = Orientation::Orthogonal;
	sf::Vector2i mapsize;
	sf::Vector2i tilesize;
	std::vector <layer_data> layers;

	std::vector<ecs::Entity_id> get_entities_at(int layer, unsigned int tile_index);
	std::vector<std::vector<ecs::Entity_id>> get_entities_at(unsigned int tile_index);
};

struct Maps
{
	std::unordered_map<std::string, Map_data> maps;
};
