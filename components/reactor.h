#pragma once
#include <string>
#include <SFML/System.hpp>
#include "ecs_types.h"
#include <span>

namespace tiled
{
	struct Property;
}

struct Shared_context;

struct Reactor
{
	void reset();
	void load (std::span<tiled::Property> props);
	void load (const std::string& key, Shared_context* context) {}

	enum class On_trigger { No_action, Change_tile, Activate };
	int id { 0 };
	int triggered_by;
	std::string tile_true;
	std::string tile_false;
	sf::Vector2i tile_coords;
	ecs::Entity_id entity;
	bool make_accessible;
	bool make_transparant;
	On_trigger action;
};