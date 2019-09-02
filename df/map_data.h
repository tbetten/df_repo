#pragma once
#include <SFML/System/Vector2.hpp>
#include <unordered_map>
#include <string>

struct Map_data
{
	enum class Orientation { Orthogonal, Hexagonal, Isometric };
	Orientation orientation = Orientation::Orthogonal;
	sf::Vector2i mapsize;
	sf::Vector2i tilesize;
};

struct Maps
{
	std::unordered_map<std::string, Map_data> maps;
};
