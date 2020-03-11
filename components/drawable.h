#pragma once

#include "ecs_types.h"
#include "SFML/Graphics.hpp"

#include <unordered_map>

namespace cache
{
	struct Cache;
	class Resource_base;
}

struct Icon
{
	std::string key = "";
	std::shared_ptr<cache::Resource_base> resource_ptr = nullptr;
	sf::Texture* texture = nullptr;
	sf::Sprite sprite;
};

namespace ecs
{
	class Entity_manager;
}

struct Drawable
{
	Drawable();
	void reset ();
	
	std::vector<std::string> locations;
	std::unordered_map<std::string, Icon> icon_parts;
	sf::Sprite sprite;
	sf::Texture* texture;
	std::unique_ptr<sf::RenderTexture> composed_icon;
	sf::Vector2f screen_coords;
	std::shared_ptr<cache::Resource_base> m_texture_resource;
	
};

void init_drawable (ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& key, cache::Cache* cache, const sf::IntRect& rect = sf::IntRect{ 0,0,32,32 });//, grid::Layout layout);

void fill_icon_part(ecs::Entity_manager* mgr, cache::Cache* cache, std::string key, const std::string& location, ecs::Entity_id entity);
void fill_icon_part(ecs::Entity_manager* mgr, std::shared_ptr<cache::Resource_base> texture_resource, sf::IntRect area, const std::string& location, ecs::Entity_id entity);
void clear_icon_part(ecs::Entity_manager* mgr, const std::string& location, ecs::Entity_id entity);