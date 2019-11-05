#pragma once
#include "system.h"
#include <SFML/System.hpp>

namespace ecs
{
	class Entity_manager;
	template <typename T>
	struct Component;
}
struct Facing;

namespace systems
{
	class Facing_system : public ecs::S_base, private messaging::Sender
	{
	public:
		Facing_system (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m);
		void update (sf::Int64 dt) override;
		void setup_events () override;
	//	Dispatcher& get_event (const std::string& event) override;
	private:
		void change_facing (std::any val);

	//	std::unordered_map<std::string, Dispatcher> m_dispatchers;
		ecs::Entity_manager* m_entity_manager;
		ecs::Component<Facing>* m_facing_comp;
	};
}