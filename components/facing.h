#pragma once
#include "directions.h"
#include "SFML/Graphics.hpp"

struct Facing
{
	Facing () { reset (); }
	void reset ();
	Compass facing = Compass::North;
	sf::Sprite facing_indicator;
	sf::Texture facing_texture;
};