#pragma once
#include "system.h"
//#include "messaging.h"
#include <optional>
#include <SFML/System.hpp>
//#include "move_payload.h"

/*namespace messaging
{
	class Messenger;
}*/

namespace event
{
	struct Event_info;
}

enum class Direction : int;

namespace systems
{
	class Controller : public ecs::S_base, private messaging::Sender
	{
	public:
		Controller (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m);
		void update (sf::Int64 dt) override;
		void setup_events () override;
//		Dispatcher& get_event (const std::string& event) override;
		std::optional<int> take_turn(unsigned int entity);
	private:
//		std::unordered_map<std::string, Dispatcher> m_dispatchers;
		ecs::Entity_id m_current_entity{ 0 };
		bool m_player_controlled{ true };
		messaging::Messenger* m_messenger;

		void move_ai(std::any val);
		void move (Direction d);
		void register_entity(std::any val);
		void get(event::Event_info data);
	};
}