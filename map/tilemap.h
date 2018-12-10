#pragma once
#include "SFML/Graphics.hpp"
#include "utils.h"
#include "shared_context.h"
#include "resource_cache.h"

struct Tilemap;
struct Layer : public sf::Drawable
{
	void create_model();

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	std::string m_name;
	std::vector<int> m_data;
	std::ptrdiff_t m_tileset_index;
	sf::VertexArray m_model;
	std::shared_ptr<cache::Resource_base> m_tileset_resource;
	Tilemap* m_map;
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
	explicit Tilemap(Shared_context* context);
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
	Shared_context* m_context;

	std::vector<Tileset_data> m_tilesets;
	std::vector <Layer> m_layers;
};
