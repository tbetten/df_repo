#pragma once
#include <string>
#include <memory>
#include <SFML/System/Vector2.hpp>

struct Tilemap;

struct Position
{
	void reset ();
	std::string map_id = "";
	sf::Vector2u coords;
	std::string current_map;
	int layer = 2;
	bool moved = false;
};

bool operator< (const Position& lhs, const Position& rhs);