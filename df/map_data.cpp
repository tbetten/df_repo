#include "map_data.h"
#include "tinyxml2.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace std::string_literals;

/*std::vector<ecs::Entity_id> Map_data::get_entities_at(const std::string& layer, unsigned int tile_index) const
{

/*	auto tile = layers[layer][tile_index];
	for (auto entity : tile)
	{
		if (entity)
		{
			result.push_back(*entity);
		}
	}
	return result;
}*/

std::vector<ecs::Entity_id> Map_data::get_entities_at(unsigned int tile_index) const
{
	std::vector <ecs::Entity_id> result;
	if (tile_index >= m_map_index.size()) return result;
	auto& cell_data = m_map_index[tile_index];
	for (auto& [layer, entities] : cell_data)
	{
		std::copy(std::rbegin(entities), std::rend(entities), std::back_inserter(result));
	}
	return result;

/*	std::vector<std::vector<ecs::Entity_id>> result;
	if (tile_index < 0 || tile_index > layers[0].size()) return result;
	for (unsigned int i = 0; i < layers.size(); ++i)
	{
		result.emplace_back(get_entities_at(i, tile_index));
	}
	return result;*/
}

Map_data::Map_data(tinyxml2::XMLElement* element)
{
	std::string orientation_param = element->Attribute("orientation");
	m_mapsize = sf::Vector2i{ element->IntAttribute("width"), element->IntAttribute("height") };
	m_tilesize = sf::Vector2i{ element->IntAttribute("tilewidth"), element->IntAttribute("tileheight") };
	if (orientation_param == "orthogonal")
	{
		m_orientation = Map_data::Orientation::Orthogonal;
		m_topology = std::make_unique<Orthogonal>(m_mapsize, m_tilesize);
	}
	if (orientation_param == "hexagonal")
	{
		m_orientation = Map_data::Orientation::Hexagonal;
	}
	if (orientation_param == "isometric")
	{
		m_orientation = Map_data::Orientation::Isometric;
	}
	m_pathfinding.reserve(m_mapsize.x * m_mapsize.y);
	lighting_map.reserve(m_mapsize.x * m_mapsize.y);
}

Map_data::Map_data(Orientation orientation, sf::Vector2i mapsize, sf::Vector2i tilesize) : m_orientation{ orientation }, m_mapsize{ mapsize }, m_tilesize{ tilesize }
{
	if (orientation == Map_data::Orientation::Orthogonal)
	{
		m_topology = std::make_unique<Orthogonal>(m_mapsize, m_tilesize);
	}
	m_map_index.reserve(m_mapsize.x* m_mapsize.y);
	for (size_t i = 0; i < m_mapsize.x * m_mapsize.y; ++i)
	{
		m_map_index.push_back(std::map<Position::Layer, std::vector<ecs::Entity_id>>{});
	}
}

Position::Layer find_layer(const Map_data::Cell& cell, ecs::Entity_id entity)
{
	for (auto& [layer, entities] : cell)
	{
		auto itr = std::find_if(std::cbegin(entities), std::cend(entities), [entity](ecs::Entity_id e) {return entity == e; });
		if (itr != std::cend(entities)) return layer;
	}
	return Position::Layer::Invalid;
}

Tile_coords Map_data::move(ecs::Entity_id entity, Tile_coords old_pos, Compass direction)
{
	auto new_pos = m_topology->neighbour(old_pos, direction);
	if (new_pos == old_pos) return new_pos;
	auto old_index = m_topology->tile_index(old_pos);// old_pos.x + old_pos.y * m_mapsize.x;
	auto new_index = m_topology->tile_index(new_pos); // new_pos.x + new_pos.y * m_mapsize.x;
	if (m_pathfinding[new_index] == -1) return old_pos;
	auto layer = find_layer(m_map_index[old_index], entity);
	if (layer == Position::Layer::Invalid) return old_pos;
	remove_entity(entity, old_pos);
	add_to_map(entity, new_index, layer);
//	auto cell = m_map_index[old_index];
//	cell.erase (std::remove_if(std::begin(cell), std::end(cell), [entity](ecs::Entity_id id) {return id == entity; }), std::end(cell));
//	m_map_index[new_index].push_back(entity);
	return new_pos;
}


void Map_data::add_to_map(ecs::Entity_id entity, int tile_index, Position::Layer layer)
{
	auto& cell = m_map_index[tile_index];
	cell[layer].push_back(entity);
}

void Map_data::remove_entity(ecs::Entity_id entity, Tile_coords coords)
{
	int index = m_topology->tile_index(coords);// coords.x + coords.y * m_mapsize.x;
	auto& cell = m_map_index[index];
	auto layer = find_layer(cell, entity);
	if (layer == Position::Layer::Invalid) return;
	auto& entities = cell.at(layer);
	//auto x = std::remove_if(std::begin(entities), std::end(entities), [entity](ecs::Entity_id id) {return entity == id; });
	//entities.erase(x, std::end(entities));
	entities.erase(std::remove_if(std::begin(entities), std::end(entities), [entity](ecs::Entity_id id) {return entity == id; }), std::end(entities));
}

Tile_coords Orthogonal::to_tile_coords(Screen_coords point) const noexcept
{
	auto map_coords = point - m_offset;
	return Tile_coords{map_coords.x / m_tile_size.x, map_coords.y / m_tile_size.y};
}

Tile_coords Orthogonal::to_tile_coords(int tile_index) const noexcept
{
	return Tile_coords{ tile_index % m_map_size.x, tile_index / m_map_size.y };
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

Screen_coords Orthogonal::center(int tile_index) const noexcept
{
	return center(to_tile_coords(tile_index));
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

Tile_coords Orthogonal::neighbour(Tile_coords tile, Compass direction) const
{
	Tile_coords res;
	switch (direction)
	{
	case Compass::North:
		res = tile + north;
		break;
	case Compass::North_east:
		res = tile + north_east;
		break;
	case Compass::East:
		res = tile + east;
		break;
	case Compass::South_east:
		res = tile + south_east;
		break;
	case Compass::South:
		res = tile + south;
		break;
	case Compass::South_west:
		res = tile + south_west;
		break;
	case Compass::West:
		res = tile + west;
		break;
	case Compass::North_west:
		res = tile + north_west;
		break;
	default:
		throw std::invalid_argument("bad direction: " + std::to_string(static_cast<int>( direction)));
	}
	return in_bounds(res) ? res : tile;
}

bool Orthogonal::is_diagonal_step(Tile_coords from, Tile_coords to) const
{
	if (!are_neighbours(from, to)) return false;
	if (from.x == to.x || from.y == to.y) return false;
	return true;
}