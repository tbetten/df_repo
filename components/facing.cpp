#include "facing.h"
#include "resource_cache.h"
#include <iostream>

void Facing::reset ()
{
	facing = Compass::North;
	m_texture_resource = nullptr;

/*	if (!facing_texture.loadFromFile ("assets/sprite/arrow_up.png"))
	{
		std::cout << "kan texture niet laden \n";
	}
	facing_indicator.setTexture (facing_texture);
	auto x = facing_texture.getSize ();
	x / 2u;
	facing_indicator.setOrigin (static_cast<sf::Vector2f>(facing_texture.getSize () / 2u));*/
}