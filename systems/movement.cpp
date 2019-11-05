#include "movement.h"
#include "components.h"
#include "component.h"
#include "position.h"
#include "facing.h"
#include "ecs.h"
#include "move_payload.h"
#include "shared_context.h"
#include "map_data.h"
#include "tile_type.h"
#include "systems.h"
#include "facing_payload.h"
//#include "tilemap.h"
#include <iostream>

namespace systems
{
	using namespace std::string_literals;

	Movement::Movement (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* messenger) : S_base (type, mgr), messaging::Sender{messenger}
	{
		ecs::Bitmask b;
		b.set (static_cast<int>(ecs::Component_type::Position));
		m_requirements.push_back (b);

		add_message("changed_facing");
//		m_dispatchers.emplace("changed_facing", Dispatcher{});
//		m_system_manager->register_events(ecs::System_type::Movement, { "changed_facing" });
	}

	void Movement::setup_events ()
	{
		m_messenger->bind("changed_position"s, [this](auto val) {move(val); });
//		auto sys = m_system_manager->find_event ("change_position");
//		m_system_manager->get_event (sys, "change_position").bind ([this](auto val) {move (val); });
	}

	void Movement::update(sf::Int64 dt)
	{}

/*	Dispatcher& Movement::get_event (const std::string& event)
	{
		return m_dispatchers[event];
	}*/

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
		auto new_facing = Compass_util::find_new_facing (payload.direction, facing->facing);
		if (facing->facing != new_facing)
		{
			std::cout << "facing changed\n";
			notify("changed_facing", Facing_payload{ entity, new_facing });
//			m_dispatchers["changed_facing"].notify(Facing_payload{ entity, new_facing });
			facing->facing = new_facing;
		}

		auto coords = pos_comp->coords;
		auto delta = Compass_util::get_direction_vector (move_dir);
		sf::Vector2u new_coords { coords.x + delta.x, coords.y + delta.y };
		auto tile_index = new_coords.y * width + new_coords.x;
		auto& map_data = context->m_maps->maps[context->m_current_map];
		auto entities = map_data.get_entities_at(tile_index);
		bool accessible{ false };
		for (auto layer : entities)
		{
			for (auto entity : layer)
			{
				if (entity_mgr->has_component(entity, ecs::Component_type::Tile_type))
				{
					auto data = entity_mgr->get_data<ecs::Component<Tile_type>>(ecs::Component_type::Tile_type, entity);
					accessible = data->accessible;
				}
			}
		}
		if (accessible)
		{
			pos_comp->coords.x = new_coords.x;
			pos_comp->coords.y = new_coords.y;
			pos_comp->moved = true;
		}
	}
}