#include "map_data.h"
#include "tinyxml2.h"
#include <iostream>
#include <stdexcept>

using namespace std::string_literals;

std::vector<ecs::Entity_id> Map_data::get_entities_at(int layer, unsigned int tile_index) const
{
	std::vector <ecs::Entity_id> result;
	auto tile = layers[layer][tile_index];
	for (auto entity : tile)
	{
		if (entity)
		{
			result.push_back(*entity);
		}
	}
	return result;
}

std::vector<std::vector<ecs::Entity_id>> Map_data::get_entities_at(unsigned int tile_index) const
{
	std::vector<std::vector<ecs::Entity_id>> result;
	if (tile_index < 0 || tile_index > layers[0].size()) return result;
	for (unsigned int i = 0; i < layers.size(); ++i)
	{
		result.emplace_back(get_entities_at(i, tile_index));
	}
	return result;
}

Map_data::Map_data(tinyxml2::XMLElement* element)
{
	std::string orientation_param = element->Attribute("orientation");
	mapsize = sf::Vector2i{ element->IntAttribute("width"), element->IntAttribute("height") };
	tilesize = sf::Vector2i{ element->IntAttribute("tilewidth"), element->IntAttribute("tileheight") };
	if (orientation_param == "orthogonal")
	{
		orientation = Map_data::Orientation::Orthogonal;
		topology = std::make_unique<Orthogonal>(mapsize, tilesize);
	}
	if (orientation_param == "hexagonal")
	{
		orientation = Map_data::Orientation::Hexagonal;
	}
	if (orientation_param == "isometric")
	{
		orientation = Map_data::Orientation::Isometric;
	}
	
	lighting_map.reserve(mapsize.x * mapsize.y);
}

Tile_coords Orthogonal::to_tile_coords(Screen_coords point) const noexcept
{
	auto map_coords = point - m_offset;
	return Tile_coords{map_coords.x / m_tile_size.x, map_coords.y / m_tile_size.y};
}

// gives top left corner
Screen_coords Orthogonal::to_screen_coords(Tile_coords tile) const noexcept
{
	return Screen_coords { tile.x * m_tile_size.x + m_offset.x, tile.y * m_tile_size.y + m_offset.x };
}

Screen_coords Orthogonal::center(Tile_coords tile) const noexcept
{
	auto topleft = to_screen_coords(tile);
	return Screen_coords{ topleft.x + (m_tile_size.x / 2), topleft.y + (m_tile_size.y / 2) };
}

bool Orthogonal::are_neighbours(Tile_coords tile1, Tile_coords tile2) const noexcept
{
	sf::Vector2i diff = sf::Vector2i{ tile1.x - tile2.x, tile1.y - tile2.y };// tile1 - tile2;
	return std::abs(diff.x) <= 1 && std::abs(diff.y) <= 1;
}

bool Orthogonal::in_bounds(Tile_coords tile) const noexcept
{
	return tile.x >= 0 && tile.y >= 0 && tile.x < m_map_size.x && tile.y < m_map_size.y;
}

std::vector<Tile_coords> Orthogonal::neighbours(Tile_coords tile) const noexcept
{
	std::vector<Tile_coords> ret;
	if (in_bounds(tile + north)) ret.push_back(tile + north);
	if (in_bounds(tile + north_east)) ret.push_back(tile + north_east);
	if (in_bounds(tile + east)) ret.push_back(tile + east);
	if (in_bounds(tile + south_east)) ret.push_back(tile + south_east);
	if (in_bounds(tile + south)) ret.push_back(tile + south);
	if (in_bounds(tile + south_west)) ret.push_back(tile + south_west);
	if (in_bounds(tile + west)) ret.push_back(tile + west);
	if (in_bounds(tile + north_west)) ret.push_back(tile + north_west);

	return ret;
}

int Orthogonal::tile_index(Tile_coords tile) const
{
	if (!in_bounds(tile)) throw std::out_of_range("coordinates ("s + std::to_string(tile.x) + ", "s + std::to_string(tile.y) + ") are out of bounds\n"s);
	return tile.y * m_map_size.x + tile.x;
}