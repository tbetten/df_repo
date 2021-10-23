#include "inventory_system.h"
#include "components.h"
#include "position.h"
#include "container.h"
#include "inventory.h"
#include "item.h"
#include "ecs.h"
#include "pickup_payload.h"
#include "shared_context.h"
#include "map_data.h"
#include "equippable.h"
#include "drawable.h"
//#include "entity_at.h"

#include <iostream>
#include <cassert>
#include <span>
#include <numeric>
#include <ctype.h>

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
		m_inventory = m_entity_mgr->get_component<ecs::Component<Inventory>> (ecs::Component_type::Inventory);

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
		auto inventory_index = m_entity_mgr->get_index(ecs::Component_type::Inventory, character);
		if (!inventory_index) return;
		std::cout << "B " << *inventory_index << "\t" << *position_index << "\n";
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
					auto& container = m_inventory->m_data[inventory_index.value()].bag;
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
		auto inventory_index = m_entity_mgr->get_index(ecs::Component_type::Inventory, holder);
		assert(inventory_index);
		auto& inventory = m_inventory->m_data[inventory_index.value()].bag;
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

	void Inventory_system::equip_items (ecs::Entity_id holder, std::span<ecs::Entity_id> items)
	{
		if (!m_entity_mgr->has_component (holder, ecs::Component_type::Drawable)) return;
		auto inventory = m_entity_mgr->get_data<ecs::Component<Inventory>> (ecs::Component_type::Inventory, holder);
		auto& inv_contents = inventory->bag.contents;
		std::vector<ecs::Entity_id> removelist;
		for (auto item : items)
		{
			if (!m_entity_mgr->has_component (item, ecs::Component_type::Equippable)) continue;
			auto equippable = m_entity_mgr->get_data<ecs::Component<Equippable>> (ecs::Component_type::Equippable, item);
			auto slot = equippable->slot;
			if (inventory->inventory [static_cast<int>(slot)]) continue;  // slot already filled
			auto itr = std::find_if (std::begin (inv_contents), std::end (inv_contents), [item] (const ecs::Entity_id& entity){return item == entity; });
			assert (itr != std::end (inv_contents));
			removelist.push_back (*itr);
			auto itr2 = std::find_if (std::cbegin (inventory::string_to_slot), std::cend (inventory::string_to_slot), [slot] (const inventory::slot& s){return s.second == slot; });
			auto slot_name = itr2->first;
			std::transform (std::begin (slot_name), std::end (slot_name), std::begin (slot_name), [] (char c){return std::tolower (c); });
			fill_icon_part (m_entity_mgr, m_system_manager->get_context ()->m_cache, equippable->tilesheet, equippable->tile_index, slot_name, holder);
			inventory->inventory [static_cast<int>(slot)] = item;
		}
		for (auto item : removelist)
		{
			std::erase (inv_contents, item);
		}
	}

	void Inventory_system::unequip_items (ecs::Entity_id holder, std::span<ecs::Entity_id>items)
	{
		auto inventory = m_entity_mgr->get_data<ecs::Component<Inventory>> (ecs::Component_type::Inventory, holder);
		auto drawable = m_entity_mgr->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, holder);
		for (auto item : items)
		{
			auto slot = std::find_if (std::begin (inventory->inventory), std::end (inventory->inventory), [item] (std::optional<ecs::Entity_id>& e){return e.has_value () && e.value () == item; });
			*slot = std::nullopt;
			auto x = std::distance (std::begin (inventory->inventory), slot);
			auto n = inventory::string_to_slot.at (x).first;
			std::transform (std::begin (n), std::end (n), std::begin (n), [] (char c){return std::tolower (c); });
			auto& icon = drawable->icon_parts.at (n);
			icon.texture = nullptr;
			m_messenger->notify ("icon_changed", holder);
			inventory->bag.contents.push_back (item);
		}
	}
}