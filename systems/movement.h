#pragma once
#include "system.h"

namespace systems
{
	class Movement : public ecs::S_base
	{
	public:
		Movement (ecs::System_type type, ecs::System_manager* mgr);
		void update (float dt) override;
		void setup_events () override;
		Dispatcher& get_event (const std::string& event) override;
	private:
		void move (std::any val);
	};
}