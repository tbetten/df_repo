#pragma once
#include "system.h"
#include <SFML/System.hpp>

namespace systems
{


	class Interaction : public ecs::S_base, private messaging::Sender
	{
	public:
		Interaction(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* messenger);
		void update(sf::Int64 dt) override;
		void setup_events() override;
		//		Dispatcher& get_event (const std::string& event) override;
	private:
		//		std::unordered_map<std::string, Dispatcher> m_dispatchers;
		void use(std::any val);
		void trigger(std::any val);
	};
}