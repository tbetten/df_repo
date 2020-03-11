#pragma once
#include "system.h"

namespace ecs
{
	class Entity_manager;
	template <typename T>
	struct Component;
}

struct Lightsource;
struct Map_data;

namespace systems
{
	class Lighting : public ecs::S_base
	{
	public:
		Lighting(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m);
		void update(sf::Int64 dt) override;
		void setup_events() override;
	private:
		messaging::Messenger* m_messenger;
		ecs::Entity_manager* m_entity_manager;
		ecs::Component<Lightsource>* m_lightsource_comp;

		bool m_first = true;

		void init_lighting_map();
	};
}