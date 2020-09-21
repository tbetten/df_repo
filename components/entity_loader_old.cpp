#ifdef BLA

#include "entity_loader_old.h"
#include "shared_context.h"
#include "resource_cache.h"
#include "components.h"
#include "drawable.h"
#include "position.h"
#include "character.h"
#include "animation.h"
#include "map_data.h"
#include "tinyxml2.h"
#include "tileset.h"
#include "tile_info.h"
#include "tile_type.h"
#include "lightsource.h"

#include <sstream>
#include <iostream>
#include <optional>
#include <charconv>

namespace ts = tileset;

namespace el_old
{
	std::vector<std::string> split(const std::string& s, const std::string& delim)
	{
		std::vector<std::string> res;
		for (auto first = s.data(), second = s.data(), last = first + s.size(); second != last && first != last; first = second + 1)
		{
			second = std::find_first_of(first, last, std::cbegin(delim), std::cend(delim));
			if (first != second)
			{
				res.emplace_back(first, second - first);
			}
		}
		return res;
	}

	Entity_loader::Entity_loader(Shared_context* context) : m_context{ context }, m_db{"./assets/database/gamedat.db" }
	{
		using namespace std::string_literals;
		m_entity_stmt = m_db.prepare("select key, components from entity where key = ?"s);
		m_filename_stmt = m_db.prepare("select file from maps where handle = ?"s);
	}

	std::optional<ecs::Entity_id> Entity_loader::load_entity(const std::string& key)
	{
		m_entity_stmt.bind(1, key);
		auto res = m_entity_stmt.execute_row();
		if (res == db::Prepared_statement::Row_result::Row)
		{
			auto row = m_entity_stmt.fetch_row();
			m_entity_stmt.reset();
			unsigned int comps = std::get<int>(row["components"]);
			auto id = m_context->m_entity_manager->add_entity(ecs::Bitmask{ comps });
			if (m_context->m_entity_manager->has_component(id, ecs::Component_type::Drawable))
			{
				init_drawable(m_context->m_entity_manager, id, key, m_context->m_cache);
			}
			return id;
		}
		return std::nullopt;
	}

	bool Entity_loader::set_position(ecs::Entity_id entity, sf::Vector2i coords, int layer, std::string map)
	{
		if (m_context->m_entity_manager->has_component(entity, ecs::Component_type::Position))
		{
			auto position = m_context->m_entity_manager->get_data<ecs::Component<Position>>(ecs::Component_type::Position, entity);
			position->coords = coords;
			position->layer = static_cast<Position::Layer>( layer);
			position->current_map = map;
			position->moved = true;
			auto tile_index = coords.y * m_context->m_maps->maps[map].m_mapsize.x + coords.x;
			auto& cell = m_context->m_maps->maps[map].layers[layer][tile_index];
			auto cell_itr = std::find_if(std::begin(cell), std::end(cell), [](std::optional<size_t> id) {return id == std::nullopt; });
			*cell_itr = entity;
			return true;
		}
		return false;
	}

	bool Entity_loader::set_player_controlled(ecs::Entity_id entity, bool player_controlled)
	{
		auto mgr = m_context->m_entity_manager;
		if (mgr->has_component(entity, ecs::Component_type::Character))
		{
			auto character = mgr->get_data<ecs::Component<Character>>(ecs::Component_type::Character, entity);
			character->user_controlled = player_controlled;
			return true;
		}
		return false;
	}

	std::string Entity_loader::get_filename(const std::string& handle) 
	{
		m_filename_stmt.bind(1, handle);
		m_filename_stmt.execute_row();
		auto row = m_filename_stmt.fetch_row();
		m_filename_stmt.reset();
		return std::get<std::string>(row["file"]);
	}

	class Next
	{
	public:
		Next(std::string_view sv, std::string_view delims_v) : s{ sv }, delims{ delims_v }, offset{ 0 }{}
		explicit Next(std::string_view delims_v) : delims{ delims_v }, offset{ 0 }{}
		void set_data(std::string_view sv) { s = sv; offset = 0; }
		void reset(std::string_view sv, std::string_view delims_v)
		{
			s = sv;
			delims = delims_v;
			offset = 0;
		}
		std::optional<unsigned int> operator()()
		{
			auto first = s.find_first_not_of(delims, offset);
			if (first == std::string_view::npos) return std::nullopt;
			auto second = s.find_first_of(delims, first);
			if (second == std::string_view::npos) second = s.size();
			offset = second;
			std::from_chars(s.data() + first, s.data() + second, result);
			return result;
		}
	private:
		std::string_view s;
		std::string_view delims;
		size_t offset;
		unsigned int result = 0;
	};

	void Entity_loader::load_map(const std::string& map_handle)
	{
		using Tileset_data = std::pair<std::shared_ptr<cache::Resource>, tileset::Tileset*>;
		std::vector<std::pair<std::shared_ptr<cache::Resource>, tileset::Tileset*>> tilesets;
//		std::vector<ts::Tileset> tilesets;
		auto cache = m_context->m_cache;

		auto objecttypes = get_filename("objecttypes");
		auto tile_types = tile_info::load_tile_data(objecttypes);
		auto filename = get_filename(map_handle);
		std::cout << filename << "\n";
		tinyxml2::XMLDocument doc;
		doc.LoadFile(filename.c_str());
		auto map_elem = doc.FirstChildElement("map");
		Map_data md{map_elem};
//		auto map_elem = doc.FirstChildElement("map");
		//md.load(map_elem);
/*		std::string orientation = map_elem->Attribute("orientation");
		md.mapsize = sf::Vector2i{ map_elem->IntAttribute("width"),  map_elem->IntAttribute("height") };
		if (orientation == "orthogonal")
		{
			md.orientation = Map_data::Orientation::Orthogonal;
		}
		if (orientation == "hexagonal")
		{
			md.orientation = Map_data::Orientation::Hexagonal;
		}
		if (orientation == "isometric")
		{
			md.orientation = Map_data::Orientation::Isometric;
		}
		md.tilesize = sf::Vector2i{ map_elem->IntAttribute("tilewidth"), map_elem->IntAttribute("tileheight") };*/
		
		auto num_cols = md.m_mapsize.x;
		for (auto tileset_elem = map_elem->FirstChildElement("tileset"); tileset_elem != nullptr; tileset_elem = tileset_elem->NextSiblingElement("tileset"))
		{
			auto source = tileset_elem->Attribute("source");
			//ts::Tileset ts{ source, tileset_elem->IntAttribute("firstgid") };
			filename = get_filename(source);
			auto tileset_resource = m_context->m_cache->get_obj(filename);
			auto tileset = cache::get<tileset::Tileset>(tileset_resource);
			//ts::Tileset ts = ts::load_tileset(filename);
			//ts.firstgid = tileset_elem->IntAttribute("firstgid");
			tileset->set_first_gid(tileset_elem->IntAttribute("firstgid"));
			tilesets.push_back(std::make_pair(tileset_resource, tileset));
//			tilesets.push_back(ts);
		}

		auto position = m_context->m_entity_manager->get_component<ecs::Component<Position>>(ecs::Component_type::Position);
		auto drawable = m_context->m_entity_manager->get_component<ecs::Component<Drawable>>(ecs::Component_type::Drawable);
		std::string delims = " ,\n\t";
		Next next(delims);

		for (auto layer_elem = map_elem->FirstChildElement("layer"); layer_elem != nullptr; layer_elem = layer_elem->NextSiblingElement("layer"))
		{
			unsigned int layer_id = layer_elem->IntAttribute("name");
			auto data_elem = layer_elem->FirstChildElement("data");
			std::string data = data_elem->GetText();
			next.set_data(data);

			while (md.layers.size() <= layer_id)
			{
				md.layers.emplace_back(Map_data::layer_data{});
			}

			int tile_index{ 0 };
			while (auto n = next())
			{
				md.layers[layer_id].emplace_back(Map_data::cell_data{});
				md.layers[layer_id].back().fill(std::nullopt);
				int gid;
				gid = n.value();
				if (gid == 0)
				{
					++tile_index;
					continue;
				}

				auto [resource, ts] = *(std::find_if(tilesets.cbegin(), tilesets.cend(), [gid](const Tileset_data& ts) 
					{
						auto [firstgid, lastgid] = ts.second->get_gids();
						return firstgid < gid && lastgid > gid; 
					}));
				auto [firstgid, lastgid] = ts->get_gids();
				int tile_id = gid - firstgid;
				auto rect = ts->get_rect(tile_id);
				ecs::Bitmask b;
				b.set(static_cast<int> (ecs::Component_type::Position));
				b.set(static_cast<int>(ecs::Component_type::Drawable));
			//	Animation anim;

				auto itr = std::find_if(ts->animations.cbegin(), ts->animations.cend(), [tile_id](ts::Animation a)
					{return std::find_if(a.cbegin(), a.cend(), [tile_id](ts::Animation_frame f) {return f.tile_id == tile_id; }) != a.cend(); });
				if (itr != ts->animations.cend())
				{
					std::cout << "2 animation found at " << tile_index << " \n";
					b.set(static_cast<int> (ecs::Component_type::Animation));
				}

				auto type_itr = std::find_if(std::cbegin(ts->tile_types), std::cend(ts->tile_types), [tile_id](ts::Tile_type t) {return t.first == tile_id; });
				if (type_itr != std::cend(ts->tile_types))
				{
					b.set(static_cast<int>(ecs::Component_type::Tile_type));
					auto x = *type_itr;
					if (x.second == "wall_lightsource") b.set(static_cast<int>(ecs::Component_type::Lightsource));
				}

				auto entity = m_context->m_entity_manager->add_entity(b);
				auto& cell = md.layers[0][tile_index];
				auto cell_itr = std::find_if(std::begin(cell), std::end(cell), [](std::optional<size_t> id) {return id == std::nullopt; });
				*cell_itr = entity;

				if (m_context->m_entity_manager->has_component(entity, ecs::Component_type::Animation))
				{
					auto anim_comp = m_context->m_entity_manager->get_data<ecs::Component<::Animation>>(ecs::Component_type::Animation, entity);
					std::cout << std::distance(ts->animations.cbegin(), itr) << "\n";
					for (auto frame : *itr)
					{
						auto r = ts->get_rect(frame.tile_id);
						anim_comp->frames.emplace_back(Frame{ ts->get_rect(frame.tile_id), frame.duration });
					}
				}
				if (m_context->m_entity_manager->has_component(entity, ecs::Component_type::Tile_type))
				{
					auto tile_type_comp = m_context->m_entity_manager->get_data<ecs::Component<::Tile_type>>(ecs::Component_type::Tile_type, entity);
					auto type = type_itr->second;
					auto ti_itr = std::find_if(std::cbegin(tile_types), std::cend(tile_types), [type](tile_info::Tile_info ti) {return ti.name == type; });
					tile_type_comp->accessible = ti_itr->accessible;
					tile_type_comp->description = ti_itr->description;
					tile_type_comp->name = ti_itr->name;
					tile_type_comp->transparent = ti_itr->transparant;
					if (type == "wall_lightsource")
					{
						auto lightsource_comp = m_context->m_entity_manager->get_data<ecs::Component<Lightsource>>(ecs::Component_type::Lightsource, entity);
						lightsource_comp->direction = ti_itr->direction;
						lightsource_comp->ignited = ti_itr->ignited;
						lightsource_comp->brightness = ti_itr->brightness;
					}
				}
				auto pos_index = *m_context->m_entity_manager->get_index(ecs::Component_type::Position, entity);
				auto drawable_index = *m_context->m_entity_manager->get_index(ecs::Component_type::Drawable, entity);
				position->m_data[pos_index].coords = sf::Vector2i{ tile_index % num_cols, tile_index / num_cols };
				position->m_data[pos_index].layer = static_cast<Position::Layer>( layer_id);
				position->m_data[pos_index].map_id = map_handle;
				//drawable->m_data[drawable_index].m_texture_resource = t.texture;
				//auto texture = cache::get_val<sf::Texture>(t.texture.get());
				//drawable->m_data[drawable_index].texture = texture;
				//drawable->m_data[drawable_index].sprite.setTexture(*texture);
				//drawable->m_data[drawable_index].sprite.setTextureRect(t.get_rect(tile_id));
				auto x = tile_index % md.m_mapsize.x * md.m_tilesize.x;
				auto y = tile_index / md.m_mapsize.y * md.m_tilesize.y;
				drawable->m_data[drawable_index].screen_coords = sf::Vector2f{ static_cast<float>(tile_index % md.m_mapsize.x * md.m_tilesize.x), static_cast<float>(tile_index / md.m_mapsize.y * md.m_tilesize.y) };
				drawable->m_data[drawable_index].sprite.setPosition(drawable->m_data[drawable_index].screen_coords);

				++tile_index;
			}
		}
		m_context->m_maps->maps.emplace(map_handle, std::move(md));
	}
}
#endif