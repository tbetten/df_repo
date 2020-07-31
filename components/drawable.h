#pragma once

#include "ecs_types.h"
#include "SFML/Graphics.hpp"

#include <unordered_map>

namespace cache
{
	struct Cache;
	class Resource_base;
}

namespace tiled
{
	class Tilesheet;
}

struct Vertex_coords
{
	sf::FloatRect coords;
	sf::FloatRect texture_coords;
};

class Vertex_array : public sf::Drawable, sf::Transformable
{
public:
	explicit Vertex_array(int number = 0) : m_array { sf::Quads, number } {}
	void add_tile(int index, sf::FloatRect coords, sf::FloatRect texture_coords);
	void add_texture(std::shared_ptr<cache::Resource_base> res, tiled::Tilesheet* sheet);
	void remove_tile(int tile_index);
	void rebuild();

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::VertexArray m_array;
	std::map<int, Vertex_coords> m_index;
	std::shared_ptr<cache::Resource_base> m_res{ nullptr };
	tiled::Tilesheet* m_tex{ nullptr };
};

struct Icon
{
	std::string key = "";
	std::shared_ptr<cache::Resource_base> resource_ptr = nullptr;
	tiled::Tilesheet* tilesheet = nullptr;
	int tile_index = 0;
	sf::Texture* texture = nullptr;
	sf::Sprite sprite;
};

namespace ecs
{
	class Entity_manager;
}

struct Drawable : public sf::Drawable
{
	enum class Type { Vertex_array, Entity };
	Drawable();
	void reset ();
	
	Type type{ Type::Entity };
	std::vector<std::string> locations;
	std::unordered_map<std::string, Icon> icon_parts;
	sf::Sprite sprite;
	sf::Texture* texture;
	std::unique_ptr<sf::RenderTexture> composed_icon;
	sf::Vector2f screen_coords;
	std::shared_ptr<cache::Resource_base> m_texture_resource;
	Vertex_array m_array;
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

void init_drawable (ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& key, cache::Cache* cache, const sf::IntRect& rect = sf::IntRect{ 0,0,32,32 });//, grid::Layout layout);
void init_drawable(ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& tilesheet, cache::Cache* cache, int tile_index);

void fill_icon_part(ecs::Entity_manager* mgr, cache::Cache* cache, std::string tilesheet, int tile_index, const std::string& location, ecs::Entity_id entity);
void fill_icon_part(ecs::Entity_manager* mgr, std::shared_ptr<cache::Resource_base> texture_resource, sf::IntRect area, const std::string& location, ecs::Entity_id entity);
void clear_icon_part(ecs::Entity_manager* mgr, const std::string& location, ecs::Entity_id entity);
void change_icon_index(ecs::Entity_manager* mgr, const std::string& location, ecs::Entity_id entity, int tile_index);
void change_icon_area(ecs::Entity_manager* mgr, const std::string& location, ecs::Entity_id entity, sf::IntRect area);