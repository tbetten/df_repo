#pragma once
#include "system.h"
#include <SFML/System.hpp>

enum class Direction : int;

namespace systems
{
	class Movement : public ecs::S_base, private messaging::Sender
	{
	public:
		Movement (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* messenger);
		void update (sf::Int64 dt) override;
		void setup_events () override;
		int move(ecs::Entity_id, Direction direction);
//		Dispatcher& get_event (const std::string& event) override;
	private:
//		std::unordered_map<std::string, Dispatcher> m_dispatchers;
		void move (std::any val);
	};
}