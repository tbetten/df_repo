#include "facing_system.h"
#include "ecs.h"
#include "components.h"
#include "position.h"
#include "drawable.h"
#include "facing.h"
#include "directions.h"

namespace systems
{
	Facing_system::Facing_system (ecs::System_type type, ecs::System_manager* mgr) : S_base (type, mgr)
	{
		ecs::Bitmask b;
		b.set (static_cast<int>(ecs::Component_type::Drawable));
		b.set (static_cast<int>(ecs::Component_type::Position));
		b.set (static_cast<int>(ecs::Component_type::Facing));
		m_requirements.push_back (b);
	}

	void Facing_system::setup_events ()
	{

	}

	void Facing_system::update(float dt)
	{
		auto entity_mgr = m_system_manager->get_entity_mgr ();
		for (auto entity : m_entities)
		{
			auto pos_comp = entity_mgr->get_data<ecs::Component<Position>> (ecs::Component_type::Position, entity);
			auto facing = entity_mgr->get_data<ecs::Component<Facing>> (ecs::Component_type::Facing, entity);
			auto drawable = entity_mgr->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
			auto coords = drawable->screen_coords;
			auto angle = Compass_util::get_direction_angle (facing->facing);
			auto vector = Compass_util::get_direction_vector (facing->facing) * 5;
			facing->facing_indicator.setRotation (static_cast<float> (angle));
			coords.x += vector.x;
			coords.y += vector.y;
			facing->facing_indicator.setPosition (coords);
		}
	}

	Dispatcher& Facing_system::get_event (const std::string& event)
	{
		throw "I don't have events";
	}
}