#include "stdafx.h"
#include "tilemap.h"
#include "tinyxml2.h"
#include "utils.h"
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

Tilemap::Tilemap() : m_orientation_map{4}, m_render_order_map{4}, m_axis_map{2}, m_stagger_index_map{2}
{
	m_orientation_map.insert(Orientation::Orthogonal, "orthogonal");
	m_orientation_map.insert(Orientation::Isometric, "isometric");
	m_orientation_map.insert(Orientation::Staggered, "staggered");
	m_orientation_map.insert(Orientation::Hexagonal, "hexagonal");

	m_render_order_map.insert(Render_order::Left_down, "left-down");
	m_render_order_map.insert(Render_order::Left_up, "left-up");
	m_render_order_map.insert(Render_order::Right_down, "right-down");
	m_render_order_map.insert(Render_order::Right_up, "right-up");

	m_axis_map.insert(Axis::X, "x");
	m_axis_map.insert(Axis::Y, "y");

	m_stagger_index_map.insert(Staggerindex::Even, "even");
	m_stagger_index_map.insert(Staggerindex::Odd, "odd");
}

void Tilemap::load_from_file(const std::string& filename)
{

	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str());
	auto map_elem = doc.FirstChildElement("map");
	m_mapsize.x = map_elem->IntAttribute("width");
	m_mapsize.y = map_elem->IntAttribute("height");
	m_tilesize.x = map_elem->IntAttribute("tilewidth");
	m_tilesize.y = map_elem->IntAttribute("tileheight");
	m_hex_side_length = map_elem->IntAttribute("hexsidelength");

	m_orientation = m_orientation_map.at(map_elem->Attribute("orientation"));
	m_render_order = m_render_order_map.at(map_elem->Attribute("renderorder"));
	m_stagger_axis = m_axis_map.at(map_elem->Attribute("staggeraxis"));
	m_stagger_index = m_stagger_index_map.at(map_elem->Attribute("staggerindex"));

	for (auto tileset_elem = map_elem->FirstChildElement("tileset"); tileset_elem != nullptr; tileset_elem = tileset_elem->NextSiblingElement("tileset"))
	{
		Tileset_data t;
		t.m_filename = tileset_elem->Attribute("source");
		t.m_first_index = tileset_elem->IntAttribute("firstgid");
		m_tilesets.push_back(t);
	}

	for (auto layer_elem = map_elem->FirstChildElement("layer"); layer_elem != nullptr; layer_elem = layer_elem->NextSiblingElement("layer"))
	{
		Layer l;
		l.m_name = layer_elem->Attribute("name");
		auto data_elem = layer_elem->FirstChildElement("data");
		std::string index_data = data_elem->GetText();
		std::istringstream ss{ index_data };
		std::string buf;
		while (std::getline(ss, buf, ','))
		{
			std::istringstream fs(buf);
			int i;
			fs >> i;
			l.m_data.push_back(i);
		}
		auto x = std::minmax_element(l.m_data.cbegin(), l.m_data.cend());
		int min_index{ 0 };
		int max_index{ 0 };
		for (auto tileset : m_tilesets)
		{
			if ()
		}
		m_layers.push_back(l);
	}
}