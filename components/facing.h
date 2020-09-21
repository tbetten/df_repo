#pragma once

#include "directions.h"
#include "SFML/Graphics.hpp"

namespace cache
{
	struct Cache;
	class Resource_base;
}

struct Shared_context;

struct Facing
{
	Facing () { reset (); }
	void reset ();
	void load (const std::string& key, Shared_context* context) {}

	Compass facing = Compass::North;
	sf::Sprite facing_indicator;
	sf::Texture* facing_texture;
	std::shared_ptr<cache::Resource_base> m_texture_resource;
};