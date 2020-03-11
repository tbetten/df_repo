#pragma once
#include "system.h"

namespace ecs
{
	template <class T>
	struct Component;
	class Entity_manager;
}

struct Position;
struct Container;

namespace systems
{
	class Inventory_system : public ecs::S_base
	{
	public:
		Inventory_system(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m);
		void update(sf::Int64 dt) override;
		void setup_events() override;
	private:
		messaging::Messenger* m_messenger;
		ecs::Entity_manager* m_entity_mgr;
		ecs::Component<Position>* m_position{ nullptr };
		ecs::Component<Container>* m_container{ nullptr };

		void pickup(std::any val);
	};
}