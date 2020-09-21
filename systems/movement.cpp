#include "movement.h"
#include "components.h"
#include "component.h"
#include "position.h"
#include "facing.h"
#include "attributes.h"
#include "character.h"
#include "ecs.h"
#include "move_payload.h"
#include "shared_context.h"
#include "map_data.h"
#include "tile_type.h"
#include "systems.h"
#include "facing_payload.h"
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
	}

	void Movement::setup_events ()
	{
//		m_messenger->bind("changed_position"s, [this](auto val) {move(val); });
	}

	void Movement::update(sf::Int64 dt)
	{}

	//void Movement::move (std::any val)
	int Movement::move (ecs::Entity_id entity, Direction direction)
	{
		int time{ 0 };
		auto context = m_system_manager->get_context();
		auto map = context->m_current_map;
		if (map == "") return 0;
		auto& map_data = context->m_maps->maps[map];
		auto& mapsize = context->m_maps->maps[map].m_mapsize;
		int width = mapsize.x;
		//auto payload = std::any_cast<Move_payload>(val);
		//auto entity = payload.entity;
		auto entity_mgr = m_system_manager->get_entity_mgr ();
		auto pos_comp = entity_mgr->get_data<ecs::Component<Position>> (ecs::Component_type::Position, entity);
		auto facing = entity_mgr->get_data<ecs::Component<Facing>> (ecs::Component_type::Facing, entity);

		auto new_facing = Compass_util::find_new_facing(direction, facing->facing);
		if (facing->facing != new_facing)
		{
			time += 50;
			std::cout << "facing changed\n";
			notify("changed_facing", Facing_payload{ entity, new_facing });
			facing->facing = new_facing;
		}
		//if (direction == Direction::Turn_left || direction == Direction::Turn_right) return time;

		Compass move_dir = Compass_util::find_move_direction (direction, facing->facing);
		auto new_pos = map_data.move(entity, pos_comp->coords , move_dir);
		if (new_pos != pos_comp->coords)
		{
			double stepsize = map_data.m_topology->is_diagonal_step(pos_comp->coords, new_pos) ? std::sqrt(2.0) : 1.0;
			pos_comp->coords = new_pos;
			pos_comp->moved = true;
			auto attributes = entity_mgr->get_data<ecs::Component<Attributes>>(ecs::Component_type::Attributes, entity);
			auto character = entity_mgr->get_data<ecs::Component<Character>>(ecs::Component_type::Character, entity);
			auto [move, _] = attributes::get_encumbered_value(attributes->transactions, character->encumbrance);
			time += static_cast<int> (stepsize * 1000 / move);
			
			
		}
		std::cout << time << "\n";
		return time;
	}
}