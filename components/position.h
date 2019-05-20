#pragma once
#include <string>
#include <memory>

#include "hexlib.h"

struct Tilemap;

struct Position
{
	void reset ();
	std::string map_id = "";
	sf::Vector2u coords;
	std::shared_ptr<Tilemap> current_map;
};