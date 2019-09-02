#pragma once
#include "system.h"
#include <SFML/System.hpp>

namespace systems
{
	class Facing_system : public ecs::S_base
	{
	public:
		Facing_system (ecs::System_type type, ecs::System_manager* mgr);
		void update (sf::Int64 dt) override;
		void setup_events () override;
		Dispatcher& get_event (const std::string& event) override;
	private:
		void change_facing ();
	};
}