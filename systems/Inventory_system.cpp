#include "inventory_system.h"
#include "components.h"
#include "position.h"
#include "container.h"
#include "ecs.h"
#include "pickup_payload.h"
#include "shared_context.h"
#include "map_data.h"
#include "entity_at.h"

#include <iostream>

namespace systems
{
	Inventory_system::Inventory_system(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m) : S_base(type, mgr), m_messenger{ m }
	{
		ecs::Bitmask b;
		b.set(static_cast<int>(ecs::Component_type::Position));
		b.set(static_cast<int>(ecs::Component_type::Container));
		m_requirements.push_back(b);

		m_entity_mgr = mgr->get_entity_mgr();
		m_position = m_entity_mgr->get_component<ecs::Component<Position>>(ecs::Component_type::Position);
		m_container = m_entity_mgr->get_component<ecs::Component<Container >> (ecs::Component_type::Container);
	}

	void Inventory_system::setup_events()
	{
		m_messenger->bind("pickup", [this](auto val) {pickup(val); });
	}

	void Inventory_system::update(sf::Int64 dt) {}

	void Inventory_system::pickup(std::any val)
	{
		auto payload = std::any_cast<Pickup_payload>(val);
		auto character = payload.entity;
		//auto coords = payload.coords;
		auto coords = sf::Vector2i{ payload.coords.x, payload.coords.y };
		auto position_index = m_entity_mgr->get_index(ecs::Component_type::Position, character);
		if (!position_index) return;
		auto container_index = m_entity_mgr->get_index(ecs::Component_type::Container, character);
		if (!container_index) return;
		std::cout << "B " << *container_index << "\t" << *position_index << "\n";
		auto& position = m_position->m_data[position_index.value()];
		auto context = m_system_manager->get_context();
		auto& topology = context->m_maps->maps[context->m_current_map].topology;
		int x = static_cast<int>(position.coords.x);
		int y = static_cast<int>(position.coords.y);
		std::cout << "(" << x << ", " << y << ")\n";
		std::cout << x - coords.x << "\t" << y - coords.y << "\n";
		//if ((std::abs(x - coords.x) > 1) || (std::abs(y - coords.y) > 1))
		if (topology->are_neighbours(position.coords, coords))
		{
			std::cout << "in range\n";
			auto& map_data = context->m_maps->maps[context->m_current_map];
			auto tile_index = topology->tile_index(coords);
			//auto entities = map_data.get_entities_at(1, tile_index);
			auto entities = get_entity_at(m_entity_mgr, context->m_current_map, 1, coords);
			if (!entities.empty())
			{
				auto entity = entities.front();
				std::cout << "entity: " << entity << "\n";
				if (m_entity_mgr->has_component(entity, ecs::Component_type::Position))
				{
					std::cout << "bla\n";
					auto& container = m_container->m_data[container_index.value()];
					container.contents.push_back(entity);
					m_entity_mgr->remove_component_from_entity(entity, ecs::Component_type::Position, true);
				}
			}
		}
		else
		{
			std::cout << "too far\n";
		}
	}
}