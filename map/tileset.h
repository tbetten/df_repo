#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>

struct Tile_property
{
	std::string name;
	std::string type;
	std::string value;
};

bool operator== (Tile_property first, Tile_property second);

using Tile_properties = std::unordered_map<std::string, Tile_property>;
using Property_list = std::vector<Tile_property>;
using Properties = std::unordered_map<int, std::unordered_map<std::string, int>>;

struct Tileset
{
	using Ptr = std::unique_ptr<Tileset>;
	static Ptr create () { return std::make_unique<Tileset> (); }
	void loadFromFile(const std::string& file);
	sf::IntRect get_tile(int  tile_index);

	std::string m_name{ "" };
	sf::Vector2i m_tilesize{ 0, 0 };
	sf::Vector2i m_tile_offset{ 0,0 };
	int m_tilecount = 0;
	int m_columns = 0;
	int m_spacing = 0;
	int m_margin = 0;
	std::string m_filename{ "" };
	Properties m_property_indices;
	Property_list m_properties;

	std::vector<sf::IntRect> m_rects;
	std::vector<std::vector<sf::Vector2i>> m_corners;

private:
	void precalculate();
};