#pragma once
#include "SFML/Graphics.hpp"
#include "utils.h"

struct Layer
{
	std::string m_name;
	std::vector<int> m_data;
};

struct Tileset_data
{
	int m_first_index;
	std::string m_filename;
};

struct Tilemap
{
	enum class Orientation { Orthogonal, Isometric, Staggered, Hexagonal };
	enum class Render_order { Right_down, Right_up, Left_down, Left_up };
	enum class Axis { X, Y };
	enum class Staggerindex { Even, Odd };
	Tilemap();
	void load_from_file(const std::string& path);

	String_bimap<Orientation> m_orientation_map;
	String_bimap<Render_order> m_render_order_map;
	String_bimap<Axis> m_axis_map;
	String_bimap<Staggerindex> m_stagger_index_map;

	sf::Vector2i m_mapsize;
	sf::Vector2i m_tilesize;
	Orientation m_orientation;
	Render_order m_render_order = Render_order::Right_down;
	int m_hex_side_length;
	Axis m_stagger_axis;
	Staggerindex m_stagger_index;

	std::vector<Tileset_data> m_tilesets;
	std::vector <Layer> m_layers;
};
