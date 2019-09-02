#pragma once
#include "system.h"
#include "ecs.h"
#include <SFML/System.hpp>

namespace systems
{
	class AI : public ecs::S_base
	{
	public:
		AI(ecs::System_type type, ecs::System_manager* mgr);
		void update(sf::Int64 dt) override;
		void setup_events() override;
		Dispatcher& get_event(const std::string& event) override;
		int take_turn(ecs::Entity_id entity);
	private:
		std::unordered_map<std::string, Dispatcher> m_dispatchers;
	};
}