#include "movement.h"
#include "components.h"
#include "component.h"
#include "position.h"
#include "facing.h"
#include "ecs.h"
#include "move_payload.h"
#include "shared_context.h"
#include "map_data.h"
//#include "tilemap.h"

namespace systems
{
	Movement::Movement (ecs::System_type type, ecs::System_manager* mgr) : S_base (type, mgr) 
	{
		ecs::Bitmask b;
		b.set (static_cast<int>(ecs::Component_type::Position));
		m_requirements.push_back (b);
	}

	void Movement::setup_events ()
	{
		auto sys = m_system_manager->find_event ("change_position");
		m_system_manager->get_event (sys, "change_position").bind ([this](auto val) {move (val); });
	}

	void Movement::update(sf::Int64 dt)
	{}

	Dispatcher& Movement::get_event (const std::string& event)
	{
		throw "don't have any events";
	}

	void Movement::move (std::any val)
	{
		auto context = m_system_manager->get_context();
		auto map = context->m_current_map;
		auto& mapsize = context->m_maps->maps[map].mapsize;
		int width = mapsize.x;
		auto payload = std::any_cast<Move_payload>(val);
		auto entity = payload.entity;
		auto entity_mgr = m_system_manager->get_entity_mgr ();
		auto pos_comp = entity_mgr->get_data<ecs::Component<Position>> (ecs::Component_type::Position, entity);
		auto facing = entity_mgr->get_data<ecs::Component<Facing>> (ecs::Component_type::Facing, entity);

		Compass move_dir = Compass_util::find_move_direction (payload.direction, facing->facing);
		facing->facing = Compass_util::find_new_facing (payload.direction, facing->facing);

		auto coords = pos_comp->coords;
		auto delta = Compass_util::get_direction_vector (move_dir);
		pos_comp->coords.x = coords.x + delta.x;
		pos_comp->coords.y = coords.y + delta.y;
		pos_comp->moved = true;
		auto tile_id = (coords.y + delta.y) * width + (coords.x + delta.x);
	/*	std::cout << coords.x + delta.x << "\t" << coords.y + delta.y << "\t" << tile_id << "\t" << map->m_tile_info[tile_id]->description << "\n";
		if (map->m_tile_info[tile_id] && map->m_tile_info[tile_id]->accessible)
		{
			pos_comp->coords.x = coords.x + delta.x;
			pos_comp->coords.y = coords.y + delta.y;
		}*/
		
	}
}