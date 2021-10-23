#pragma once
#include "system.h"
#include <span>

namespace ecs
{
	template <class T>
	struct Component;
	class Entity_manager;
}

struct Position;
struct Inventory;

namespace systems
{
	class Inventory_system : public ecs::S_base, private messaging::Sender
	{
	public:
		Inventory_system(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m);
		void update(sf::Int64 dt) override;
		void setup_events() override;
		void drop_item(ecs::Entity_id holder, ecs::Entity_id item);
		void equip_items (ecs::Entity_id holder, std::span<ecs::Entity_id> items);
		void unequip_items (ecs::Entity_id holder, std::span<ecs::Entity_id> items);
	private:
		messaging::Messenger* m_messenger;
		ecs::Entity_manager* m_entity_mgr;
		ecs::Component<Position>* m_position{ nullptr };
		ecs::Component<Inventory>* m_inventory{ nullptr };

		void pickup(std::any val);
	};
}