#include "stdafx.h"
#include "tileset.h"
#include "tinyxml2.h"
#include <filesystem>
#include <array>

namespace fs = std::filesystem;

void Tileset::loadFromFile(const std::string& file)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(file.c_str());
	auto tileset_elem = doc.FirstChildElement("tileset");
	m_name = tileset_elem->Attribute("name");
	m_tilesize.x = tileset_elem->IntAttribute("tilewidth");
	m_tilesize.y = tileset_elem->IntAttribute("tileheight");
	m_tilecount = tileset_elem->IntAttribute("tilecount");
	m_columns = tileset_elem->IntAttribute("columns");

	auto tile_offset_elem = tileset_elem->FirstChildElement("tileoffset");
	m_tile_offset.x = tile_offset_elem->IntAttribute("x");
	m_tile_offset.y = tile_offset_elem->IntAttribute("y");

	auto image_elem = tileset_elem->FirstChildElement("image");
	std::string name = image_elem->Attribute("source");
	fs::path path{ file };
	fs::path dir = path.parent_path() / name;
	m_tiles.loadFromFile(dir.string());

	precalculate();
}

void Tileset::precalculate()
{
	for (int i = 0; i < m_tilecount; ++i)
	{
		int x_xcoord = i % m_columns;
		int y_coord = i / m_columns;
		int left = (x_xcoord * m_tilesize.x) + m_margin;
		int top = (y_coord * m_tilesize.y) + m_margin;
		m_rects.push_back (sf::IntRect{ left, top, m_tilesize.x, m_tilesize.y });
		std::vector<sf::Vector2i> corners{ sf::Vector2i(left, top), sf::Vector2i(left + m_tilesize.x, top), sf::Vector2i(left + m_tilesize.x, top + m_tilesize.y), sf::Vector2i(left, top + m_tilesize.y) };
		m_corners.push_back (corners);
	}
}

sf::IntRect Tileset::get_tile(int tile_index)
{
	return m_rects[tile_index];
}