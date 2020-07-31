#include "ai.h"
#include "ecs.h"
#include "components.h"
#include "controller.h"
#include "systems.h"
#include "directions.h"
#include "movement.h"

namespace systems
{
	AI::AI(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m) : S_base{ type, mgr }, messaging::Sender{ m }
	{
		ecs::Bitmask b;
		b.set(static_cast<int>(ecs::Component_type::Character));
		m_requirements.push_back(b);
		add_message("move");
//		m_dispatchers.emplace("move", Dispatcher{});
//		m_system_manager->register_events(ecs::System_type::AI, { "move" });
	}

	void AI::setup_events()
	{}

	void AI::update(sf::Int64 dt)
	{}

/*	Dispatcher& AI::get_event(const std::string& event)
	{
		return m_dispatchers[event];
	}
*/
	int AI::take_turn(ecs::Entity_id entity)
	{
		auto movement = m_system_manager->get_system<Movement>(ecs::System_type::Movement);
		auto time = movement->move(entity, Direction::Backward);
		//notify("move", Direction::Backward);
		//m_dispatchers["move"].notify(Direction::Backward);
		return time;
	}
}
