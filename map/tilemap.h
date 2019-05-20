#pragma once
#include "SFML/Graphics.hpp"
#include "utils.h"
#include "resource_cache.h"
#include <unordered_map>

struct Tilemap;

struct Model : public sf::Drawable, public sf::Transformable
{
	void draw (sf::RenderTarget& target, sf::RenderStates states) const override;

	std::vector<int> m_tile_indices;
	sf::VertexArray m_vertices;
	sf::Texture* m_texture;
	bool drawme{ false };
};

struct Layer : public sf::Drawable, public sf::Transformable
{
	void create_models();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	std::string m_name;
	std::vector<int> m_data;
	Tilemap* m_map;
	std::unordered_map<int, Model> m_models;
};

struct Tileset_data
{
	using Resource = std::shared_ptr<cache::Resource_base>;
	int m_first_gid;
	int m_last_gid;
	std::string m_path;
	Tileset* m_tileset_metadata;
	sf::Texture* m_texture;
	Resource m_texture_resource;
//	Resource m_tileset;
};

struct Layout
{
	sf::Vector2f origin;
	sf::Vector2u gridsize;
	sf::Vector2f tilesize;
};

namespace grid
{
	class Grid;
}

struct Shared_context;

struct Tilemap : public sf::Drawable, public sf::Transformable
{
	enum class Orientation { Orthogonal, Isometric, Staggered, Hexagonal };
	enum class Render_order { Right_down, Right_up, Left_down, Left_up };
	enum class Axis { X, Y };
	enum class Staggerindex { Even, Odd };
	explicit Tilemap(Shared_context* context);
	void load_from_file(const std::string& path);
	int num_layers () const;
	void draw_layer (sf::RenderWindow* target, int layer_index) const;
	sf::Vector2f grid_to_pixel (sf::Vector2u coords);

	void draw (sf::RenderTarget& target, sf::RenderStates states) const override;

	String_bimap<Orientation> m_orientation_map;
	String_bimap<Render_order> m_render_order_map;
	String_bimap<Axis> m_axis_map;
	String_bimap<Staggerindex> m_stagger_index_map;

	Orientation m_orientation;
	Render_order m_render_order = Render_order::Right_down;
	int m_hex_side_length;
	Axis m_stagger_axis;
	Staggerindex m_stagger_index;
	Shared_context* m_context;
	std::unique_ptr<grid::Grid> m_grid;
	Layout m_layout;

	std::vector<Tileset_data> m_tilesets;
	std::vector <Layer> m_layers;
private:
	void toggle_grid ();
	bool show_grid{ true };
};
