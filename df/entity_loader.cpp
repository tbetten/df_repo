#include "entity_loader.h"
#include "db.h"
//#include "tilemap.h"
#include "resource_cache.h"
#include "ecs.h"
#include "map_data.h"
#include "shared_context.h"
#include "ecs.h"
#include "components.h"
#include "drawable.h"
#include "position.h"
#include "animation.h"
#include "item.h"
#include "sensor.h"
#include "reactor.h"
#include "property.h"
#include "db_queries.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>

#pragma warning (disable:26812)

namespace fs = std::filesystem;
using namespace std::string_literals;

namespace el
{
	

	std::string get_filename(const std::string& handle)
	{
		//db::DB_connection connection{ "./assets/database/gamedat.db" };
		auto connection = db::DB_connection::create ("./assets/database/gamedat.db");
		db::Prepared_statement stmt = connection->prepare("select file from maps where handle = ?"s);
		stmt.bind(1, handle);
		stmt.execute_row();
		auto row = stmt.fetch_row();
		stmt.reset();
		return std::get<std::string>(row["file"]);
	}

	Map_data::Orientation convert_orientation(tiled::Map_metadata::Orientation o)
	{
		switch (o)
		{
		case tiled::Map_metadata::Orientation::Orthogonal:
			return Map_data::Orientation::Orthogonal;
		case tiled::Map_metadata::Orientation::Isometric:
			return Map_data::Orientation::Isometric;
		case::tiled::Map_metadata::Orientation::Hexagonal:
			return Map_data::Orientation::Hexagonal;
		default:
			return Map_data::Orientation::Orthogonal;
		}
	}

	sf::Vector2i convert_vector(tiled::Vector_2i v)
	{
		return sf::Vector2i{ v.x, v.y };
	}

	Position::Layer convert_layer(const std::string_view layer)
	{
		if (layer == "interactive") return Position::Layer::Interactive_objects;
		if (layer == "floor") return Position::Layer::Floor;
		if (layer == "floor_decoration") return Position::Layer::Floor_decoration;
		if (layer == "on_floor") return Position::Layer::On_floor;
		if (layer == "creature") return Position::Layer::Creature;
		if (layer == "wall") return Position::Layer::Wall;
		if (layer == "wall_decoration") return Position::Layer::Wall_decoration;
		return Position::Layer::Invalid;
	}

/*	Reactor::On_trigger convert_action(const std::string_view action)
	{
		if (action == "activate") return Reactor::On_trigger::Activate;
		if (action == "change_tile") return Reactor::On_trigger::Change_tile;
		return Reactor::On_trigger::No_action;
	}*/

	void fill_position(Position* position, const std::string_view layer, sf::Vector2i coords, const std::string_view map_handle)
	{
		position->current_map = map_handle;
		position->layer = convert_layer(layer);
		position->moved = true;
		position->coords = coords;
	}

	void fill_position(Position* position, tiled::Tile& tile, const std::string_view map_handle, int map_width)
	{
		fill_position(position, tile.layer, sf::Vector2i{ tile.map_index % map_width, tile.map_index / map_width }, map_handle);
	}



	Tile_type tile_type_to_enum(const std::string_view type)
	{
		if (type == "inert_tile") return Tile_type::Inert_tile;
		if (type == "dynamic_tile") return Tile_type::Dynamic_tile;
		if (type == "item") return Tile_type::Item;
		return Tile_type::Invalid;
	}

	void Entity_loader::create_inert_tile(tiled::Tilemap& map, tiled::Tile& tile, const std::string& map_name)
	{
		auto key = tile.layer + "_" + tile.tilesheet_name;
		if (!m_arrays.contains(key)) m_arrays.emplace(key, Array_data{ Vertex_array{}, tile.tilesheet_name, map_name ,tile.layer });
		auto& va = m_arrays[key];
		auto tilesheet  = map.tilesets[tile.tileset_index].sheet;
		auto map_size = map.metadata.map_size;
		tile.map_index;
		auto coords = sf::FloatRect{ static_cast<float>((tile.map_index % map_size.x) * tilesheet.m_tilesize.x), static_cast<float>((tile.map_index / map_size.y) * tilesheet.m_tilesize.y), static_cast<float>(tilesheet.m_tilesize.x), static_cast<float>(tilesheet.m_tilesize.y) };
		va.array.add_tile(tile.map_index, coords, sf::FloatRect{ static_cast<float>(tile.area.left), static_cast<float>(tile.area.top), static_cast<float>(tile.area.width), static_cast<float>(tile.area.height) });
	}

	std::tuple<ecs::Entity_id, Position*, Drawable*, Animation*> make_entity(ecs::Entity_manager* mgr, bool position = true, bool drawable = true, bool animation = false)
	{
		ecs::Bitmask b;
		if (position) b.set(static_cast<size_t>(ecs::Component_type::Position));
		if (drawable) b.set(static_cast<size_t> (ecs::Component_type::Drawable));
		if (animation) b.set(static_cast<size_t>(ecs::Component_type::Animation));
		auto entity = mgr->add_entity(b);
		auto position_ptr = position ? mgr->get_data<ecs::Component<Position>>(ecs::Component_type::Position, entity) : nullptr;
		auto drawable_ptr = drawable ? mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity) : nullptr;
		auto animation_ptr = animation ? mgr->get_data<ecs::Component<Animation>>(ecs::Component_type::Animation, entity) : nullptr;
		return std::make_tuple(entity, position_ptr, drawable_ptr, animation_ptr);
	}

	void Entity_loader::finalize_inert_tiles(Map_data* md)
	{
		for (auto [key, array_data] : m_arrays)
		{
			array_data.array.rebuild();
			//auto res = m_context->m_cache->get_obj(array_data.tilesheet);
			//auto tex = cache::get_val<tiled::Tilesheet>(res.get());
			//auto tex = cache::get<tiled::Tilesheet> (res);
			//auto tex = m_context->m_cache->get<tiled::Tilesheet> (array_data.tilesheet);
			//array_data.array.
			array_data.array.add_texture(m_context->m_cache->get<tiled::Tilesheet> (array_data.tilesheet));
			auto [entity, position, drawable, animation] = make_entity(m_context->m_entity_manager);
			fill_position(position, array_data.layer, sf::Vector2i{ 0,0 }, array_data.map);
			drawable->type = Drawable::Type::Vertex_array;
			drawable->m_array = std::move(array_data.array);
			//md->m_map_index[0].push_back(entity);
			md->add_to_map(entity, 0, position->layer);
		}
		m_arrays.clear();
	}

	ecs::Entity_id Entity_loader::create_dynamic_tile(tiled::Tilemap& map, tiled::Tile& tile, const std::string& map_name, Map_data* md, bool position_i, bool drawable_i)
	{
		auto mgr = m_context->m_entity_manager;
		auto [entity, position, drawable, animation] = make_entity(m_context->m_entity_manager, position_i, drawable_i, tile.animation.has_value());
		if (tile.animation.has_value())
		{
			auto& source = tile.animation.value();
			std::vector<Frame> frames {};
			std::transform (std::cbegin (source), std::cend (source), std::back_inserter (frames), [sheet = map.tilesets [tile.tileset_index].sheet] (tiled::Animation_frame f){return Frame { sheet.get_rect (f.tile_id), f.duration }; });
			animation->load (frames);
		}
		if (position) fill_position(position, tile, map_name, map.metadata.map_size.x);
		if (drawable) init_drawable(mgr, entity, tile.tilesheet_name, m_context->m_cache, tile.area);

		if (position) md->add_to_map(entity, tile.map_index, position->layer);
		return entity;
	}

	void Entity_loader::load_item_properties(ecs::Entity_id entity, const std::string& handle)
	{
		if (handle == "") return;
		auto em = m_context->m_entity_manager;
		auto [name, description, weight, value] = DB_queries::query_equipment(handle);
		if (!em->has_component(entity, ecs::Component_type::Item))
		{
			em->add_component_to_entity(entity, ecs::Component_type::Item);
		}
		auto item_comp = em->get_data<ecs::Component<Item>>(ecs::Component_type::Item, entity);
		item_comp->item_name = name;
		item_comp->description = description;
		item_comp->value = value;
		item_comp->weight = weight;
	}

	ecs::Entity_id Entity_loader::create_item(tiled::Tilemap& map, tiled::Tile& tile, const std::string& map_name, Map_data* md)
	{
		auto [entity, position, drawable, _] = make_entity(m_context->m_entity_manager);
		fill_position(position, tile, map_name, map.metadata.map_size.x);
		init_drawable(m_context->m_entity_manager, entity, tile.tilesheet_name, m_context->m_cache, tile.area);
		md->add_to_map(entity, tile.map_index, position->layer);
//		auto& cell = md->m_map_index[tile.map_index];
//		cell.push_back(entity);
		auto a = tile.properties;
		std::cout << tiled::get_property_value<std::string>(tile.properties, "item_handle").value() << "\n";

		auto key = tiled::get_property_value<std::string> (tile.properties, "item_handle").value_or ("");
		//auto db = db::DB_connection (m_context->m_database_path);
		auto db = db::DB_connection::create (m_context->m_database_path);
		auto stmt = db->prepare ("select id from components inner join entity_component on components.name = entity_component.component where entity_component.entity = ?");
		stmt.bind (1, key);
		auto table = stmt.fetch_table ();
		for (auto row : table)
		{
			auto t = std::get<int>(row ["id"]);
			assert (t >= 0);
			std::cout << key << "\t" << t << "\n";
			size_t component_id = static_cast<size_t>(t);
			if (!m_context->m_entity_manager->has_component (entity, static_cast<ecs::Component_type>(component_id)))
			{
				if (!m_context->m_entity_manager->add_component_to_entity (entity, static_cast<ecs::Component_type>(component_id), key, false))
				{
					auto index = m_context->m_entity_manager->get_index (static_cast<ecs::Component_type>(component_id), entity);
					auto component_base = m_context->m_entity_manager->get_component_by_id (component_id);
					component_base->m_context = m_context;
					component_base->load (index.value (), key);
				}
			}
			
			
			
		}

/*		load_item_properties(entity, tiled::get_property_value<std::string>(tile.properties, "item_handle").value_or(""));
		m_context->m_entity_manager->add_component_to_entity(entity, ecs::Component_type::Item);
		auto item = m_context->m_entity_manager->get_data<ecs::Component<Item>>(ecs::Component_type::Item, entity);
		item->item_name = tiled::get_property_value<std::string>(tile.properties, "item_handle").value();*/
		return entity;
	}

	ecs::Entity_id Entity_loader::create_entity(tiled::Tilemap& map, tiled::Tile& tile, Tile_type type, const std::string& map_name, Map_data* md)
	{
		switch (type)
		{
		case Tile_type::Dynamic_tile:
			return create_dynamic_tile(map, tile, map_name, md);
			break;
		case Tile_type::Inert_tile:
			create_inert_tile(map, tile, map_name);
			return 0;
			break;
		case Tile_type::Item:
			return create_item(map, tile, map_name, md);
			break;
		default:
			return 0;
			break;
		}
	}

	std::vector<int> find_tiles(sf::IntRect coords, sf::Vector2i mapsize, const Topology& top)
	{
		std::vector<int> res;
		for (int index = 0; index < mapsize.x * mapsize.y; ++index)
		{
			if (coords.contains(top.center(index))) res.push_back(index);
		}
		return res;
	}

/*	Sensor::Trigger convert_trigger(std::string_view trigger)
	{
		if (trigger == "use") return Sensor::Trigger::Use;
		if (trigger == "use_with_item") return Sensor::Trigger::Use_with_item;
		if (trigger == "enter") return Sensor::Trigger::Enter;
		return Sensor::Trigger::Invalid;
	}*/

	void Entity_loader::load_map(const std::string& handle)
	{
		auto filename = get_filename(handle);
		auto full_path = fs::current_path() / filename;
		tiled::Tilemap map{};
		map.load(full_path.string());
		Map_data md{ convert_orientation(map.metadata.orientation), convert_vector(map.metadata.map_size), convert_vector(map.metadata.tile_size) };
		ecs::Entity_manager* mgr = m_context->m_entity_manager;

		int c { 0 };
		for (auto& tile : map.tiles)
		{
			try
			{
				auto type = tile_type_to_enum (tiled::get_property_value<std::string> (tile.properties, "tile_type").value ());

				create_entity (map, tile, type, handle, &md);
			}
			catch (std::bad_optional_access& e)
			{
				std::cout << e.what() << std::to_string (c) << "\n";
			}
			++c;

		}
		finalize_inert_tiles(&md);
	//	auto coll = map.objectgroups["collision"];
		for (auto i = 0; i < md.m_mapsize.x * md.m_mapsize.y; ++i)
		{
			md.m_pathfinding.push_back(0);
		}
/*		for (auto object : coll)
		{
			auto tiles = find_tiles(object.coords, md.m_mapsize, *md.m_topology);
			std::for_each(std::cbegin(tiles), std::cend(tiles), [this, &md](int tile) {md.m_pathfinding[tile] = -1; });
		}*/
		auto entity_manager = m_context->m_entity_manager;
		for (auto [objectgroup, objects] : map.objectgroups)
		{
			for (auto& object : objects)
			{
				sf::IntRect coords{ object.coords.left, object.coords.top - object.coords.height, object.coords.width, object.coords.height };
				auto tiles = find_tiles(object.coords, md.m_mapsize, *md.m_topology);
				if (objectgroup == "collision")
				{
					std::for_each(std::cbegin(tiles), std::cend(tiles), [this, &md](int tile) {md.m_pathfinding[tile] = -1; });
				}
				else
				{
					auto props = object.properties;
					//auto entity = entity_manager->add_entity(ecs::Bitmask{});
					auto position = tiled::get_property_value<bool>(props, "position").value_or(false);
					auto drawable = tiled::get_property_value<bool>(props, "drawable").value_or(false);
					auto sensor = tiled::get_property_value<bool>(props, "sensor").value_or(false);
					auto reactor = tiled::get_property_value<bool>(props, "reactor").value_or(false);
					std::optional<ecs::Entity_id> entity{ std::nullopt };

					if (drawable)
					{
						auto layer = tiled::get_property_value<std::string>(props, "layer").value_or("invalid");
						auto index = tiles.empty() ? 0 : tiles[0];
						auto tile = map.gid_to_tile(object.gid, layer, index);
						entity = create_dynamic_tile(map, tile, handle, &md, position, drawable);
					}
					if (!entity.has_value())
					{
						entity = entity_manager->add_entity(ecs::Bitmask{});
					}
					if (position && !entity_manager->has_component(*entity, ecs::Component_type::Position))
					{
						if (tiles.empty()) throw std::out_of_range{ "can not fill position component if there are no coordinates" };
						entity_manager->add_component_to_entity(*entity, ecs::Component_type::Position, "", false);
						auto pos_comp = entity_manager->get_data<ecs::Component<Position>>(ecs::Component_type::Position, *entity);
						auto layer = tiled::get_property_value<std::string>(props, "layer").value_or("interactive");
						
						fill_position(pos_comp, layer, md.m_topology->to_tile_coords(tiles[0]), handle);
					}

					if (sensor)
					{
						entity_manager->add_component_to_entity(*entity, ecs::Component_type::Sensor, "", false);
						auto sensor_comp = entity_manager->get_data<ecs::Component<Sensor>>(ecs::Component_type::Sensor, *entity);
						props.emplace_back ("object_id", object.id);
						sensor_comp->load (props);
					}

					if (reactor)
					{
						entity_manager->add_component_to_entity(*entity, ecs::Component_type::Reactor, "", false);
						auto reactor_comp = entity_manager->get_data<ecs::Component<Reactor>>(ecs::Component_type::Reactor, *entity);
						props.emplace_back ("entity", entity.value ());
						props.emplace_back ("object_id", object.id);
						reactor_comp->load (props);
					}

					if (!tiles.empty()&& entity.has_value())
					{
						Position::Layer layer = Position::Layer::Interactive_objects;
						if (entity_manager->has_component(*entity, ecs::Component_type::Position))
						{
							auto pos = entity_manager->get_data<ecs::Component<Position>>(ecs::Component_type::Position, *entity);
							layer = pos->layer;
						}
						std::for_each(std::cbegin(tiles), std::cend(tiles), [&md, entity, layer](int tile) {md.add_to_map(*entity, tile, layer); });
					}
				}
			}
		}

/*		auto sensors = map.objectgroups["sensors"];
		for (auto object : sensors)
		{
			auto tiles = find_tiles(object.coords, md.m_mapsize, *md.m_topology);
		//	if (tiles.empty()) continue;
			auto props = object.properties;
			auto prop_itr = std::find_if(std::cbegin(props), std::cend(props), [](const tiled::Property& prop) {return prop.m_name == "type"; });
			if (prop_itr == std::cend(props)) continue;
			auto type = tiled::get_property_value<std::string>(props, "type");
			if (!type) continue;
			if (*type == "sensor")
			{
				ecs::Bitmask b;
				b.set(static_cast<size_t>(ecs::Component_type::Position));
				b.set(static_cast<size_t>(ecs::Component_type::Sensor));
				auto enitity = m_context->m_entity_manager->add_entity(b);
				auto pos = m_context->m_entity_manager->get_data<ecs::Component<Position>>(ecs::Component_type::Position, enitity);
				auto sensor = m_context->m_entity_manager->get_data<ecs::Component<Sensor>>(ecs::Component_type::Sensor, enitity);
				fill_position(pos, "floor", md.m_topology->to_tile_coords(tiles[0]), handle);
				sensor->active = tiled::get_property_value<bool>(props, "active").value();
				sensor->id = object.id;// tiled::get_property_value<std::string>(props, "id").value();
				sensor->state = tiled::get_property_value<bool>(props, "state").value();
				sensor->trigger = convert_trigger(tiled::get_property_value<std::string>(props, "activation").value());
				for (auto tile : tiles)
				{
					md.m_map_index[tile].push_back(enitity);
				}
			}
			if (*type == "reactor")
			{
				ecs::Bitmask b;
				b.set(static_cast<size_t>(ecs::Component_type::Reactor));
				auto entity = m_context->m_entity_manager->add_entity(b);
				auto reactor = m_context->m_entity_manager->get_data<ecs::Component<Reactor>>(ecs::Component_type::Reactor, entity);
				reactor->id = tiled::get_property_value<std::string>(props, "id").value();
				reactor->triggered_by = tiled::get_property_value<std::string>(props, "triggered_by").value();
				reactor->tile_false = tiled::get_property_value<std::string>(props, "tile_false").value();
				reactor->tile_true = tiled::get_property_value<std::string>(props, "tile_true").value();
				auto x = tiled::get_property_value<int>(props, "tile_x").value();
				auto y = tiled::get_property_value<int>(props, "tile_y").value();
				reactor->tile_coords = sf::Vector2i{ x,y };
			}
		}*/
		m_context->m_maps->maps.emplace(handle, std::move(md));
	}

	Position::Layer get_layer(ecs::Entity_manager* mgr, ecs::Entity_id entity)
	{
		auto position = mgr->get_data<ecs::Component<Position>>(ecs::Component_type::Position, entity);
		return position->layer;
	}

	bool Entity_loader::set_position(ecs::Entity_id entity, sf::Vector2i coords, Position::Layer layer, std::string map)
	{
		if (m_context->m_entity_manager->has_component(entity, ecs::Component_type::Position))
		{
			auto position = m_context->m_entity_manager->get_data<ecs::Component<Position>>(ecs::Component_type::Position, entity);
			position->coords = coords;
			position->layer = layer;
			position->current_map = map;
			position->moved = true;
			auto tile_index = coords.y * m_context->m_maps->maps[map].m_mapsize.x + coords.x;
			auto& md = m_context->m_maps->maps[map];
			md.add_to_map(entity, tile_index, layer);
//			auto& cell_data = m_context->m_maps->maps[map].m_map_index[tile_index];
//			auto cell_itr = std::find_if(std::begin(cell_data), std::end(cell_data), [this, layer](ecs::Entity_id id) {return static_cast<int>(get_layer(m_context->m_entity_manager, id)) < static_cast<int>(layer); });
//			cell_data.insert(cell_itr, entity);
//			*cell_itr = entity;
			return true;
		}
		return false;
	}
}