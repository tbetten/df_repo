#pragma once
#include <SFML/Graphics.hpp>

struct Tileset
{
	void loadFromFile(const std::string& file);
	sf::IntRect get_tile(int  tile_index);

	sf::Texture m_tiles;
	std::string m_name{ "" };
	sf::Vector2i m_tilesize{ 0, 0 };
	sf::Vector2i m_tile_offset{ 0,0 };
	int m_tilecount = 0;
	int m_columns = 0;
	int m_spacing = 0;
	int m_margin = 0;

	std::vector<sf::IntRect> m_rects;
	std::vector<std::vector<sf::Vector2i>> m_corners;

private:
	void precalculate();
};