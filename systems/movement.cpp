#include "movement.h"
#include "components.h"
#include "component.h"
#include "position.h"
#include "facing.h"
#include "ecs.h"
#include "move_payload.h"

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

	void Movement::update(float dt)
	{}

	Dispatcher& Movement::get_event (const std::string& event)
	{
		throw "don't have any events";
	}

	void Movement::move (std::any val)
	{
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
	}
}