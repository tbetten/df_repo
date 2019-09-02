#pragma once

#include "ecs_types.h"
#include "SFML/Graphics.hpp"

namespace cache
{
	struct Cache;
	class Resource_base;
}

namespace ecs
{
	class Entity_manager;
}

struct Drawable
{
	void reset ();
	
	sf::Sprite sprite;
	sf::Texture* texture;
	sf::Vector2f screen_coords;
	std::shared_ptr<cache::Resource_base> m_texture_resource;
};

void init_drawable (ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& key, cache::Cache* cache, const sf::IntRect& rect = sf::IntRect{ 0,0,32,32 });//, grid::Layout layout);

