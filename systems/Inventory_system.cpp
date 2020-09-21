#include "inventory_system.h"
#include "components.h"
#include "position.h"
#include "container.h"
#include "item.h"
#include "ecs.h"
#include "pickup_payload.h"
#include "shared_context.h"
#include "map_data.h"
#include "armour.h"
#include "drawable.h"
//#include "entity_at.h"

#include <iostream>
#include <cassert>
#include <span>
#include <numeric>

namespace systems
{
	Inventory_system::Inventory_system(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m) : S_base(type, mgr), messaging::Sender{ m }, m_messenger{ m }
	{
		ecs::Bitmask b;
		b.set(static_cast<int>(ecs::Component_type::Position));
		b.set(static_cast<int>(ecs::Component_type::Container));
		m_requirements.push_back(b);

		m_entity_mgr = mgr->get_entity_mgr();
		m_position = m_entity_mgr->get_component<ecs::Component<Position>>(ecs::Component_type::Position);
		m_container = m_entity_mgr->get_component<ecs::Component<Container >> (ecs::Component_type::Container);

		add_message("encumbrance_changed");
		add_message ("inventory_changed");
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
		auto& topology = context->m_maps->maps[context->m_current_map].m_topology;
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
			auto entities = map_data.get_entities_at(tile_index);
			//auto entities = get_entity_at(m_entity_mgr, context->m_current_map, Position::Layer::On_floor, coords);
			if (!entities.empty())
			{
				auto itr = std::remove_if(std::begin(entities), std::end(entities), [this](ecs::Entity_id entity) 
					{
						auto index = m_entity_mgr->get_index(ecs::Component_type::Position, entity);
						auto& pos = m_position->m_data[index.value()];
						return pos.layer != Position::Layer::On_floor;
					});
				entities.erase(itr, std::end(entities));
				if (entities.empty()) return;
				auto entity = entities.front();
				std::cout << "entity: " << entity << "\n";
				int weight{ 0 };
				if (m_entity_mgr->has_component(entity, ecs::Component_type::Item))
				{
					auto item = m_entity_mgr->get_data<ecs::Component<Item>>(ecs::Component_type::Item, entity);
					weight = item->weight;
				}
				if (m_entity_mgr->has_component(entity, ecs::Component_type::Position))
				{
					auto& container = m_container->m_data[container_index.value()];
					map_data.remove_entity(entity, coords);
					container.contents.push_back (entity);
					container.total_weight += weight;
					m_entity_mgr->remove_component_from_entity(entity, ecs::Component_type::Position, true);
					notify ("encumbrance_changed", character);
					notify ("inventory_changed", character);
				}
			}
		}
		else
		{
			std::cout << "too far\n";
		}
	}

	void Inventory_system::drop_item(ecs::Entity_id holder, ecs::Entity_id item)
	{
		auto inventory_index = m_entity_mgr->get_index(ecs::Component_type::Container, holder);
		assert(inventory_index);
		auto& inventory = m_container->m_data[inventory_index.value()];
		auto itr = std::find_if(std::begin(inventory.contents), std::end(inventory.contents), [item](ecs::Entity_id id) {return id == item; });
		if (itr != std::end(inventory.contents))
		{
			auto item_data = m_entity_mgr->get_data<ecs::Component<Item>>(ecs::Component_type::Item, item);
			inventory.total_weight -= item_data->weight;
			notify("encumbrance_changed", holder);
			inventory.contents.erase(itr);

			auto context = m_system_manager->get_context();
			auto& map_data = context->m_maps->maps[context->m_current_map];
			auto& map_index = map_data.m_map_index;

			// need coordinates where to drop; is coordinates of present character
			auto char_pos = m_entity_mgr->get_data<ecs::Component<Position>>(ecs::Component_type::Position, holder);
			auto index = map_data.m_topology->tile_index(char_pos->coords);

			// insert into map
			map_data.add_to_map(item, index, Position::Layer::On_floor);

			// dropped item now has a position in the map, so add position component to it in the ECS
			m_entity_mgr->add_component_to_entity(item, ecs::Component_type::Position);
			auto pos = m_entity_mgr->get_data<ecs::Component<Position>>(ecs::Component_type::Position, item);
			pos->coords = char_pos->coords;
			pos->current_map = context->m_current_map;
			pos->layer = Position::Layer::On_floor;
			pos->moved = true;
		}
	}

	void Inventory_system::equip_item (ecs::Entity_id holder, ecs::Entity_id item)
	{
		if (m_entity_mgr->has_component (item, ecs::Component_type::Armour) && m_entity_mgr->has_component(holder, ecs::Component_type::Drawable))
		{
			auto armour_data = m_entity_mgr->get_data<ecs::Component<Armour>> (ecs::Component_type::Armour, item);
			fill_icon_part (m_entity_mgr, m_system_manager->get_context ()->m_cache, armour_data->tilesheet, armour_data->tile_index, "torso", holder);
		}
	}
}