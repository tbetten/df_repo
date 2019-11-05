#include "entity_loader.h"
#include "shared_context.h"
#include "resource_cache.h"
#include "components.h"
#include "drawable.h"
#include "position.h"
#include "character.h"
#include "animation.h"
#include "map_data.h"
#include "tinyxml2.h"
#include "tile_info.h"
#include "tile_type.h"

#include <filesystem>
#include <sstream>
#include <iostream>
#include <optional>
#include <charconv>

namespace fs = std::filesystem;

namespace el
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

	Entity_loader::Entity_loader(Shared_context* context) : m_context{ context }
	{
		using namespace std::string_literals;
		fs::path path = fs::current_path() / "assets/database/gamedat.db";
		m_db = std::move(db::db_connection::create(path.string()));
		m_entity_stmt = m_db->prepare("select key, components from entity where key = ?"s);
		m_filename_stmt = m_db->prepare("select file from maps where handle = ?"s);
	}

	std::optional<ecs::Entity_id> Entity_loader::load_entity(const std::string& key)
	{
		m_entity_stmt->bind(1, key);
		auto res = m_entity_stmt->execute_row();
		if (res == db::Result_code::Row)
		{
			auto row = m_entity_stmt->fetch_row();
			m_entity_stmt->reset();
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

	bool Entity_loader::set_position(ecs::Entity_id entity, sf::Vector2u coords, int layer, std::string map)
	{
		if (m_context->m_entity_manager->has_component(entity, ecs::Component_type::Position))
		{
			auto position = m_context->m_entity_manager->get_data<ecs::Component<Position>>(ecs::Component_type::Position, entity);
			position->coords = coords;
			position->layer = layer;
			position->current_map = map;
			position->moved = true;
			auto tile_index = coords.y * m_context->m_maps->maps[map].mapsize.x + coords.x;
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

	struct Animation_frame
	{
		int tile_id = 0;
		int duration = 0;
	};

	using Tile_type = std::pair<int, std::string>;
	using Tile_types = std::vector<Tile_type>;
	using Animation = std::vector<Animation_frame>;
	using Animations = std::vector<Animation>;

	struct Tileset
	{
		Tileset() :name{ "" }, firstgid{ 0 }, lastgid{ 0 }, tilewidth{ 0 }, tileheight{ 0 }, texture{ nullptr }, columns{ 0 }, source{ "" } {}
		Tileset(std::string source_, int firstgid_) : source{ std::move(source_) }, firstgid{ firstgid_ }, name{ "" }, lastgid{ 0 }, tilewidth{ 0 }, tileheight{ 0 }, texture{ nullptr }, columns{ 0 }{}
		sf::IntRect get_rect(int tile_id);
		std::string name;
		int firstgid;
		int lastgid;
		int tilewidth;
		int tileheight;
		std::shared_ptr<cache::Resource_base> texture;
		int columns;
		std::string source;
		Tile_types tile_types;
		Animations animations;
	};

	sf::IntRect Tileset::get_rect(int tile_id)
	{
		auto x = tile_id % columns;
		auto y = tile_id / columns;
		auto pixel_x = x * tilewidth;
		auto pixel_y = y * tileheight;
		return sf::IntRect{ (tile_id % columns) * tilewidth, (tile_id / columns) * tileheight, tilewidth, tileheight };
	}

	void load_tileset(std::string_view filename, Tileset& tileset, Shared_context* context)
	{
		tinyxml2::XMLDocument doc;
		doc.LoadFile(filename.data());
		auto tileset_elem = doc.FirstChildElement("tileset");
		tileset.columns = tileset_elem->IntAttribute("columns");
		tileset.lastgid = tileset.firstgid + tileset_elem->IntAttribute("tilecount") - 1;
		tileset.name = tileset_elem->Attribute("name");
		tileset.tilewidth = tileset_elem->IntAttribute("tilewidth");
		tileset.tileheight = tileset_elem->IntAttribute("tileheight");
		auto image_elem = tileset_elem->FirstChildElement("image");
		tileset.source = image_elem->Attribute("source");
		tileset.texture = context->m_cache->get_obj(tileset.source);

		for (auto tile_elem = tileset_elem->FirstChildElement("tile"); tile_elem != nullptr; tile_elem = tile_elem->NextSiblingElement("tile"))
		{
			auto id = tile_elem->IntAttribute("id");
			if (tile_elem->Attribute("type"))
			{
				std::string type = tile_elem->Attribute("type");
				tileset.tile_types.push_back(std::make_pair(id, type));
			}
			auto animation_elem = tile_elem->FirstChildElement("animation");
			if (animation_elem)
			{
				Animation a{};
				for (auto frame_elem = animation_elem->FirstChildElement("frame"); frame_elem != nullptr; frame_elem = frame_elem->NextSiblingElement("frame"))
				{
					Animation_frame  af;
					af.tile_id = frame_elem->IntAttribute("tileid");
					af.duration = frame_elem->IntAttribute("duration");
					a.push_back(af);
				}
				tileset.animations.push_back(a);
			}
		}
	}

	std::string Entity_loader::get_filename(const std::string& handle) const
	{
		m_filename_stmt->bind(1, handle);
		m_filename_stmt->execute_row();
		auto row = m_filename_stmt->fetch_row();
		m_filename_stmt->reset();
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
		std::vector<Tileset> tilesets;
		auto cache = m_context->m_cache;

		auto objecttypes = get_filename("objecttypes");
		auto tile_types = tile_info::load_tile_data(objecttypes);
		auto filename = get_filename(map_handle);
		std::cout << filename << "\n";
		tinyxml2::XMLDocument doc;
		doc.LoadFile(filename.c_str());
		Map_data md;
		auto map_elem = doc.FirstChildElement("map");
		std::string orientation = map_elem->Attribute("orientation");
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
		md.tilesize = sf::Vector2i{ map_elem->IntAttribute("tilewidth"), map_elem->IntAttribute("tileheight") };
		
		auto num_cols = md.mapsize.x;
		for (auto tileset_elem = map_elem->FirstChildElement("tileset"); tileset_elem != nullptr; tileset_elem = tileset_elem->NextSiblingElement("tileset"))
		{
			auto source = tileset_elem->Attribute("source");
			Tileset ts{ source, tileset_elem->IntAttribute("firstgid") };
			filename = get_filename(source);
			load_tileset(filename, ts, m_context);
			tilesets.push_back(ts);
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

				auto t = *(std::find_if(tilesets.cbegin(), tilesets.cend(), [gid](const Tileset& ts) {return ts.firstgid < gid && ts.lastgid > gid; }));
				int tile_id = gid - t.firstgid;
				auto rect = t.get_rect(tile_id);
				ecs::Bitmask b;
				b.set(static_cast<int> (ecs::Component_type::Position));
				b.set(static_cast<int>(ecs::Component_type::Drawable));
			//	Animation anim;

				auto itr = std::find_if(t.animations.cbegin(), t.animations.cend(), [tile_id](Animation a)
					{return std::find_if(a.cbegin(), a.cend(), [tile_id](Animation_frame f) {return f.tile_id == tile_id; }) != a.cend(); });
				if (itr != t.animations.cend())
				{
					std::cout << "2 animation found at " << tile_index << " \n";
					b.set(static_cast<int> (ecs::Component_type::Animation));
				}

				auto type_itr = std::find_if(std::cbegin(t.tile_types), std::cend(t.tile_types), [tile_id](Tile_type t) {return t.first == tile_id; });
				if (type_itr != std::cend(t.tile_types))
				{
					b.set(static_cast<int>(ecs::Component_type::Tile_type));
				}

				auto entity = m_context->m_entity_manager->add_entity(b);
				auto& cell = md.layers[0][tile_index];
				auto cell_itr = std::find_if(std::begin(cell), std::end(cell), [](std::optional<size_t> id) {return id == std::nullopt; });
				*cell_itr = entity;

				if (m_context->m_entity_manager->has_component(entity, ecs::Component_type::Animation))
				{
					auto anim_comp = m_context->m_entity_manager->get_data<ecs::Component<::Animation>>(ecs::Component_type::Animation, entity);
					std::cout << std::distance(t.animations.cbegin(), itr) << "\n";
					for (auto frame : *itr)
					{
						auto r = t.get_rect(frame.tile_id);
						anim_comp->frames.emplace_back(Frame{ t.get_rect(frame.tile_id), frame.duration });
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
				}
				auto pos_index = *m_context->m_entity_manager->get_index(ecs::Component_type::Position, entity);
				auto drawable_index = *m_context->m_entity_manager->get_index(ecs::Component_type::Drawable, entity);
				position->m_data[pos_index].coords = sf::Vector2u{ static_cast<unsigned int>(tile_index % num_cols), static_cast<unsigned int> (tile_index / num_cols) };
				position->m_data[pos_index].layer = layer_id;
				position->m_data[pos_index].map_id = map_handle;
				drawable->m_data[drawable_index].m_texture_resource = t.texture;
				auto texture = cache::get_val<sf::Texture>(t.texture.get());
				drawable->m_data[drawable_index].texture = texture;
				drawable->m_data[drawable_index].sprite.setTexture(*texture);
				drawable->m_data[drawable_index].sprite.setTextureRect(t.get_rect(tile_id));
				auto x = tile_index % md.mapsize.x * md.tilesize.x;
				auto y = tile_index / md.mapsize.y * md.tilesize.y;
				drawable->m_data[drawable_index].screen_coords = sf::Vector2f{ static_cast<float>(tile_index % md.mapsize.x * md.tilesize.x), static_cast<float>(tile_index / md.mapsize.y * md.tilesize.y) };
				drawable->m_data[drawable_index].sprite.setPosition(drawable->m_data[drawable_index].screen_coords);

				++tile_index;
			}
		}
		m_context->m_maps->maps.emplace(map_handle, md);
	}
}