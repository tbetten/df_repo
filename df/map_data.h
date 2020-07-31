#pragma once
#include <SFML/System/Vector2.hpp>
#include <unordered_map>
#include <map>
#include <vector>
#include <array>
#include <string>
#include <optional>
#include <memory>

#include "directions.h"
#include "ecs_types.h"
#include "position.h"

namespace tinyxml2
{
	class XMLElement;
}

using Tile_coords = sf::Vector2i;
using Screen_coords = sf::Vector2i;

class Topology
{
public:
	using Ptr = std::unique_ptr<Topology>;
	virtual ~Topology() = default;

	virtual Tile_coords to_tile_coords(Screen_coords point) const noexcept = 0;
	virtual Tile_coords to_tile_coords(int tile_index) const noexcept = 0;
	virtual Screen_coords to_screen_coords(Tile_coords tile) const noexcept = 0;
	virtual Screen_coords center(Tile_coords tile) const noexcept = 0;
	virtual Screen_coords center(int tile_index) const noexcept = 0;
	virtual bool are_neighbours(Tile_coords tile1, Tile_coords tile2) const noexcept = 0;
	virtual std::vector<Tile_coords> neighbours(Tile_coords tile) const noexcept = 0;
	virtual bool in_bounds(Tile_coords tile) const noexcept = 0;
	virtual int tile_index(Tile_coords tile) const = 0;
	virtual Tile_coords neighbour(Tile_coords tile, Compass direction) const = 0;
	virtual bool is_diagonal_step(Tile_coords from, Tile_coords to) const = 0;
	inline void set_offset(Screen_coords offset) noexcept { m_offset = offset; }

protected:
	Screen_coords m_offset;
};

class Orthogonal : public Topology
{
public:
	Orthogonal(Tile_coords map_size, Screen_coords tile_size) : m_map_size{ map_size }, m_tile_size{ tile_size }{}
	Tile_coords to_tile_coords(Screen_coords point) const noexcept override;
	Tile_coords to_tile_coords(int tile_index) const noexcept override;
	Screen_coords to_screen_coords(Tile_coords tile) const noexcept override;
	Screen_coords center(Tile_coords tile) const noexcept override;
	Screen_coords center(int tile_index) const noexcept override;
	bool are_neighbours(Tile_coords tile1, Tile_coords tile2) const noexcept override;
	std::vector<Tile_coords> neighbours(Tile_coords tile) const noexcept override;
	bool in_bounds(Tile_coords tile) const noexcept override;
	int tile_index(Tile_coords tile) const override;
	Tile_coords neighbour(Tile_coords tile, Compass direction) const override;
	bool is_diagonal_step(Tile_coords from, Tile_coords to) const override;
private:

	Tile_coords m_map_size;
	Screen_coords m_tile_size;
	Screen_coords m_offset;

	Tile_coords north{ 0, -1 };
	Tile_coords north_east{ 1, -1 };
	Tile_coords east{ 1,0 };
	Tile_coords south_east{ 1,1 };
	Tile_coords south{ 0,1 };
	Tile_coords south_west{ -1,1 };
	Tile_coords west{ -1,0 };
	Tile_coords north_west{ -1,-1 };
};

struct Map_data
{
	enum class Orientation { Orthogonal, Hexagonal, Isometric };

	Map_data() = default;
	explicit Map_data(tinyxml2::XMLElement* element);
	Map_data(Orientation orientation, sf::Vector2i mapsize, sf::Vector2i tilesize);
	using cell_data = std::array<std::optional<ecs::Entity_id>, 10>;
	using layer_data = std::vector<cell_data>;

	using Cell = std::map<Position::Layer, std::vector<ecs::Entity_id>>;

	Orientation m_orientation = Orientation::Orthogonal;
	Topology::Ptr m_topology;
	sf::Vector2i m_mapsize;
	sf::Vector2i m_tilesize;
	std::vector<Cell> m_map_index;
	std::vector<int> m_pathfinding;
	std::vector <layer_data> layers;

	//void load(tinyxml2::XMLElement* element);

	//std::vector<ecs::Entity_id> get_entities_at(const std::string& layer, unsigned int tile_index) const;
	std::vector<ecs::Entity_id> get_entities_at(unsigned int tile_index) const;
	void add_to_map(ecs::Entity_id entity, int tile_index, Position::Layer layer);
	Tile_coords move(ecs::Entity_id entity, Tile_coords old_pos, Compass direction);
	void remove_entity(ecs::Entity_id, Tile_coords coords);

	std::vector<int> lighting_map;
};

struct Maps
{
	std::unordered_map<std::string, Map_data> maps;
};
