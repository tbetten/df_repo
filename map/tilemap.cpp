#include "stdafx.h"
#include "tilemap.h"
#include "tinyxml2.h"
#include "utils.h"
#include "grid.h"
#include <filesystem>
#include <sstream>
#include "eventmanager.h"
#include "resource_cache.h"
#include "shared_context.h"

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

	auto eventmgr = m_context->m_event_manager;
	eventmgr->add_command (Game_state::Game, "CMD_grid_toggle", [this](auto data) {toggle_grid(); });
}

void Tilemap::toggle_grid ()
{
	show_grid = !show_grid;
}

void Tilemap::load_from_file (const std::string& filename)
{

	tinyxml2::XMLDocument doc;
	doc.LoadFile (filename.c_str ());
	auto map_elem = doc.FirstChildElement ("map");
	m_hex_side_length = map_elem->IntAttribute ("hexsidelength");

	m_layout.gridsize.x = map_elem->IntAttribute ("width");
	m_layout.gridsize.y = map_elem->IntAttribute ("height");
	m_layout.tilesize.x = static_cast<float>(map_elem->IntAttribute ("tilewidth"));
	m_layout.tilesize.y = static_cast<float>(map_elem->IntAttribute ("tileheight"));
	m_layout.origin = sf::Vector2f{ 0.0f, 0.0f };
	m_grid = std::make_unique <grid::Grid> (m_layout);

	m_orientation = m_orientation_map.at (map_elem->Attribute ("orientation"));
	m_render_order = m_render_order_map.at (map_elem->Attribute ("renderorder"));
	if (map_elem->Attribute ("staggeraxis"))
	{
		m_stagger_axis = m_axis_map.at (map_elem->Attribute ("staggeraxis"));
	}
	if (map_elem->Attribute ("staggerindex"))
	{
		m_stagger_index = m_stagger_index_map.at (map_elem->Attribute ("staggerindex"));
	}
	//	m_stagger_axis = m_axis_map.at(map_elem->Attribute("staggeraxis"));
	//	m_stagger_index = m_stagger_index_map.at(map_elem->Attribute("staggerindex"));

	fs::path tilesetpath { filename };
	fs::path dir = tilesetpath.parent_path ();

	for (auto tileset_elem = map_elem->FirstChildElement("tileset"); tileset_elem != nullptr; tileset_elem = tileset_elem->NextSiblingElement("tileset"))
	{
		Tileset_data t;
		auto filename = tileset_elem->Attribute ("source");
		t.m_first_gid = tileset_elem->IntAttribute ("firstgid");
		auto cache = m_context->m_cache;
		//auto res = cache->get_obj (filename);
		auto tileset_resource = cache->get_obj (filename);
		t.m_tileset_metadata = cache::get_val <Tileset> (tileset_resource.get ());
		t.m_last_gid = t.m_first_gid + t.m_tileset_metadata->m_tilecount;
		t.m_texture_resource = cache->get_obj (t.m_tileset_metadata->m_filename);
		t.m_texture = cache::get_val<sf::Texture> (t.m_texture_resource.get ());
		m_tilesets.push_back(t);
	}

	for (auto layer_elem = map_elem->FirstChildElement("layer"); layer_elem != nullptr; layer_elem = layer_elem->NextSiblingElement("layer"))
	{
		Layer l;
		for (size_t i = 0; i < m_tilesets.size (); ++i)
		{
			l.m_models.emplace (i, Model{});
		}
		l.m_map = this;
		l.m_name = layer_elem->Attribute("name");
		auto data_elem = layer_elem->FirstChildElement("data");
		std::string index_data = data_elem->GetText();
		std::istringstream ss{ index_data };
		std::string buf;
		while (std::getline (ss, buf, ','))
		{
			std::istringstream fs (buf);
			int gid;
			fs >> gid;
			l.m_data.push_back (gid);
		}

		for (auto gid : l.m_data)
		{
			for (size_t tileset_index{ 0 }; tileset_index < m_tilesets.size (); ++tileset_index)
			{
				int min = m_tilesets[tileset_index].m_first_gid;
				int max = m_tilesets[tileset_index].m_last_gid;
				auto& data = l.m_models[tileset_index].m_tile_indices;
				if (gid == 0)
				{
					data.push_back (0);
				}
				else
				{
					if (std::clamp (gid, min, max) == gid)
					{
						data.push_back (gid - min);
					}
					else
					{
						data.push_back (0);
					}
				}
			}
		}


		l.create_models();
		m_layers.push_back(l);
	}
}

int Tilemap::num_layers () const
{
	return m_layers.size ();
}

void Tilemap::draw_layer (sf::RenderWindow* target, int layer_index) const
{
	if (layer_index > m_layers.size ()) return;
	target->draw (m_layers[layer_index]);
}

void Tilemap::draw (sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform ();

	for (auto layer : m_layers)
	{
		target.draw (layer);
	}
	if (show_grid)
	{
		target.draw (*m_grid);
	}
}

sf::Vector2f Tilemap::grid_to_pixel (sf::Vector2u coords)
{
	return grid::grid_to_pixel (m_layout, coords);
}

void Layer::create_models()
{
	for (size_t index{ 0 }; index < m_models.size (); ++index)
	{
		auto& model = m_models[index];
		model.m_texture = m_map->m_tilesets[index].m_texture;
		int num_tiles = std::count_if (model.m_tile_indices.cbegin (), model.m_tile_indices.cend (), [](int index) {return index != 0; });
		if (num_tiles == 0) continue;
		model.drawme = true;
		model.m_vertices.resize (num_tiles * 4);
		model.m_vertices.setPrimitiveType (sf::Quads);
		sf::Vertex* quad = &(model.m_vertices[0]);
		auto mapsize = m_map->m_layout.gridsize;
		auto tilesize = m_map->m_layout.tilesize;
		auto sheetsize = m_map->m_tilesets[index].m_tileset_metadata->m_columns;
		for (size_t tile_index{ 0 }; tile_index < model.m_tile_indices.size (); ++tile_index)
		{
			if (model.m_tile_indices[tile_index] == 0) continue;
			auto map_x = tile_index % mapsize.x;
			auto map_y = tile_index / mapsize.x;
			auto texture_x = model.m_tile_indices[tile_index] % sheetsize;
			auto texture_y = model.m_tile_indices[tile_index] / sheetsize;
			quad[0].position = sf::Vector2f{ static_cast<float>(map_x * tilesize.x), static_cast<float>(map_y * tilesize.y) };
			quad[1].position = sf::Vector2f{ static_cast<float>((map_x + 1) * tilesize.x), static_cast<float>(map_y * tilesize.y) };
			quad[2].position = sf::Vector2f{ static_cast<float>((map_x + 1) * tilesize.x), static_cast<float>((map_y + 1) * tilesize.y) };
			quad[3].position = sf::Vector2f{ static_cast<float>(map_x * tilesize.x), static_cast<float>((map_y + 1) * tilesize.y) };

			quad[0].texCoords = sf::Vector2f{ static_cast<float>(texture_x * tilesize.x), static_cast<float>(texture_y * tilesize.y) };
			quad[1].texCoords = sf::Vector2f{ static_cast<float>((texture_x + 1) * tilesize.x), static_cast<float>(texture_y * tilesize.y) };
			quad[2].texCoords = sf::Vector2f{ static_cast<float>((texture_x + 1) * tilesize.x), static_cast<float>((texture_y + 1) * tilesize.y) };
			quad[3].texCoords = sf::Vector2f{ static_cast<float>(texture_x * tilesize.x), static_cast<float>((texture_y + 1) * tilesize.y) };
			
			quad += 4;
		}
	}
}

void Layer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform ();

	for (auto model : m_models)
	{
		target.draw (model.second, states);
	}
}

void Model::draw (sf::RenderTarget& target, sf::RenderStates states) const
{
	if (drawme)
	{
		states.transform *= getTransform ();
		states.texture = m_texture;
		target.draw (m_vertices, states);
	}
}