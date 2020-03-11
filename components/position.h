#pragma once
#include <string>
#include <memory>
#include <SFML/System/Vector2.hpp>

struct Tilemap;

struct Position
{
	void reset ();
	std::string map_id = "";
	sf::Vector2i coords;
	std::string current_map;
	int layer = 2;
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