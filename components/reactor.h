#pragma once
#include <string>
#include <SFML/System.hpp>
#include "ecs_types.h"

struct Reactor
{
	void reset();

	enum class On_trigger { No_action, Change_tile, Activate };
	int id;
	int triggered_by;
	std::string tile_true;
	std::string tile_false;
	sf::Vector2i tile_coords;
	ecs::Entity_id entity;
	bool make_accessible;
	bool make_transparant;
	On_trigger action;
};