#pragma once
#include <string>
#include <memory>
#include <SFML/System/Vector2.hpp>

struct Tilemap;
struct Shared_context;

struct Position
{
	enum class Layer { Invalid = -1, Interactive_objects, Floor, Floor_decoration, On_floor, Creature, Wall, Wall_decoration };
	void reset ();
	void load (const std::string& key, Shared_context* context) {}

	std::string map_id = "";
	sf::Vector2i coords;
	std::string current_map;
	Layer layer;
	bool moved = false;
};

struct Position_render_order
{
	bool operator()(Position p1, Position p2)
	{
		if (p1.current_map < p2.current_map) return true;
		if (p1.layer < p2.layer) return true;
		if (p1.coords.y < p2.coords.y) return true;
		return p1.coords.x < p2.coords.x;
	}
};

bool operator< (const Position& lhs, const Position& rhs);