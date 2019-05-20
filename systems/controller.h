#pragma once
#include "system.h"
#include "messaging.h"
#include <optional>
//#include "move_payload.h"

enum class Direction : int;

namespace systems
{
	class Controller : public ecs::S_base
	{
	public:
		Controller (ecs::System_type type, ecs::System_manager* mgr);
		void update (float dt) override;
		void setup_events () override;
		Dispatcher& get_event (const std::string& event) override;
		std::optional<int> take_turn(unsigned int entity);
	private:
		std::unordered_map<std::string, Dispatcher> m_dispatchers;
		ecs::Entity_id m_current_entity{ 0 };

		void move (Direction d);
		void register_entity(std::any val);
	};
}