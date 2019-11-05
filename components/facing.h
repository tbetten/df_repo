#pragma once

#include "directions.h"
#include "SFML/Graphics.hpp"

namespace cache
{
	struct Cache;
	class Resource_base;
}

struct Facing
{
	Facing () { reset (); }
	void reset ();
	Compass facing = Compass::North;
	sf::Sprite facing_indicator;
	sf::Texture* facing_texture;
	std::shared_ptr<cache::Resource_base> m_texture_resource;
};