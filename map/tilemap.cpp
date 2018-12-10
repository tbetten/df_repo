#include "stdafx.h"
#include "tilemap.h"
#include "tinyxml2.h"
#include "utils.h"
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

Tilemap::Tilemap(Shared_context* context) : m_context{ context }, m_orientation_map { 4 }, m_render_order_map{ 4 }, m_axis_map{ 2 }, m_stagger_index_map{ 2 }
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

	std::vector<std::pair<int, int>> tileset_bounds;
	for (auto tileset : m_tilesets)
	{

		if (!tileset_bounds.empty())
		{
			tileset_bounds.back().second = tileset.m_first_index;
		}
		tileset_bounds.push_back(std::make_pair(tileset.m_first_index, -1));
	}
	for (auto layer_elem = map_elem->FirstChildElement("layer"); layer_elem != nullptr; layer_elem = layer_elem->NextSiblingElement("layer"))
	{
		Layer l;
		l.m_map = this;
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
		std::vector<int> not_zero;
		std::copy_if(l.m_data.cbegin(), l.m_data.cend(), std::back_inserter(not_zero), [](int val) {return val != 0; });

		auto x = std::minmax_element(not_zero.cbegin(), not_zero.cend());
		
		int index{ 0 };
		for (auto bound : tileset_bounds)
		{
			if (bound.second == -1)
			{
				if (*x.first > bound.first && *x.second > bound.first)
				{
					l.m_tileset_index = index;
					break;
				}
			}
			if (std::clamp(*x.first, bound.first, bound.second) == *x.first)
			{
				if (std::clamp(*x.second, bound.first, bound.second) == *x.second)
				{
					l.m_tileset_index = index;
					break;
				}
			}
			++index;
		}
		l.create_model();
		m_layers.push_back(l);
	}
}

void Layer::create_model()
{
	m_model.setPrimitiveType(sf::Quads);
	m_model.resize(m_data.size() * 4);
	auto texture_name = m_map->m_tilesets[m_tileset_index].m_filename;
	auto context = m_map->m_context;
	auto cache = context->m_cache;
	m_tileset_resource = cache->get_obj(texture_name);
	auto tileset = cache::get_val<Tileset>(m_tileset_resource.get());
	sf::Vertex* quad = &m_model[0];
	sf::Vector2f stagger_offset{ 0.0f, 0.0f };
	bool stagger{ false };
	sf::Vector2<double> hexsize {m_map->m_hex_side_length * 1.5, m_map->m_hex_side_length * std::sqrt(3.0) };

	for (auto index = 0; index < m_data.size(); ++index)
	{
		int raw_index = m_data[index];
		int first = m_map->m_tilesets[m_tileset_index].m_first_index;
		int texture_index = raw_index - first;
		if (raw_index == 0) continue;
	//	sf::Vertex* quad = &m_model[index * 4];
		int map_x = index % m_map->m_mapsize.x;
		int map_y = index / m_map->m_mapsize.x;
		if (map_x == 49)
		{
			auto a{ 0 };
		}
		auto tilesize = tileset->m_tilesize;

		auto min_x = map_x * (hexsize.x - tileset->m_tile_offset.x);
		auto min_y = map_y * (hexsize.y - tileset->m_tile_offset.y);

		if ((m_map->m_stagger_index == Tilemap::Staggerindex::Even && index % 2 == 0) || (m_map->m_stagger_index == Tilemap::Staggerindex::Odd && index % 2 != 0))
		{
			min_y += hexsize.y / 2.0;// -6.0;
			//min_y -= tileset->m_tile_offset.y;
		//	offset.y = stagger_offset.y;
		}

		quad[0].position = sf::Vector2f(min_x, min_y);
		quad[1].position = sf::Vector2f(min_x + tilesize.x, min_y);
		quad[2].position = sf::Vector2f(min_x + tilesize.x, min_y + tilesize.y);
		quad[3].position = sf::Vector2f(min_x, min_y + tilesize.y);

		
		auto corners = tileset->m_corners[texture_index];
		quad[0].texCoords = sf::Vector2f(corners[0].x, corners[0].y);
		quad[1].texCoords = sf::Vector2f(corners[1].x, corners[1].y);
		quad[2].texCoords = sf::Vector2f(corners[2].x, corners[2].y);
		quad[3].texCoords = sf::Vector2f(corners[3].x, corners[3].y);
		quad += 4;
	}
}

void Layer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	auto tileset = cache::get_val<Tileset>(m_tileset_resource.get());
	states.texture = &tileset->m_tiles;
	target.draw(m_model, states);
}