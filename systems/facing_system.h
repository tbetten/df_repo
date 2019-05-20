#pragma once
#include "system.h"

namespace systems
{
	class Facing_system : public ecs::S_base
	{
	public:
		Facing_system (ecs::System_type type, ecs::System_manager* mgr);
		void update (float dt) override;
		void setup_events () override;
		Dispatcher& get_event (const std::string& event) override;
	private:
		void change_facing ();
	};
}