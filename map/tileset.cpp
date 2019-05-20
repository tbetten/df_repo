#include "stdafx.h"
#include "tileset.h"
#include "tinyxml2.h"
#include <filesystem>
#include <array>
#include <cstdlib>

namespace fs = std::filesystem;

bool operator== (Tile_property first, Tile_property second)
{
	return first.name == second.name && first.type == second.type && first.value == second.value;
}

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
	if (tile_offset_elem)
	{
		m_tile_offset.x = tile_offset_elem->IntAttribute ("x");
		m_tile_offset.y = tile_offset_elem->IntAttribute ("y");
	}

	auto image_elem = tileset_elem->FirstChildElement("image");
	m_filename = image_elem->Attribute ("source");

	auto tile_elem = tileset_elem->FirstChildElement("tile");
	while (tile_elem != nullptr)
	{
		auto id = std::atoi(tile_elem->Attribute ("id"));
		auto properties_elem = tile_elem ->FirstChildElement("properties");
		auto property_elem = properties_elem->FirstChildElement ("property");
		while (property_elem != nullptr)
		{
			Tile_property p;
			p.name = property_elem->Attribute ("name");
			p.value = property_elem->Attribute ("value");

			auto proptype = property_elem->Attribute ("type");
			if (proptype == nullptr)
			{
				p.type = "string";
			}
			else
			{
				p.type = proptype;
			}
			auto itr = std::find (m_properties.begin (), m_properties.end (), p);
			if (itr == m_properties.end())
			{
				m_properties.push_back (p);
				itr = m_properties.end () - 1;
				
			}
			m_property_indices[id][p.name] = std::distance (m_properties.begin (), itr);

			property_elem = property_elem->NextSiblingElement ("property");
		}
		tile_elem = tile_elem->NextSiblingElement ("tile");
	}
//	precalculate();
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