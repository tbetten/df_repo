#pragma once
#include "system.h"

namespace ecs
{
	class Entity_manager;
	template <typename T>
	struct Component;
}

namespace systems
{
	class Encumbrance : public ecs::S_base, private messaging::Sender
	{
	public:
		Encumbrance(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* msg);
		void update(sf::Int64 dt) override;
		void setup_events() override;
	private:
		ecs::Entity_manager* m_entity_manager;
		void change_encumbrance(std::any val);
	};
}