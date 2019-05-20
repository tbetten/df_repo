#pragma once

#include "ecs_types.h"
#include "grid.h"
#include "SFML/Graphics.hpp"

namespace cache
{
	struct Cache;
}

namespace ecs
{
	class Entity_manager;
}

struct Drawable
{
	void reset ();
	sf::Sprite sprite;
	sf::Texture texture;
	sf::Vector2f screen_coords;
};

void init_drawable (ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& key, cache::Cache* cache);//, grid::Layout layout);