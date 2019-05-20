/*#include "stdafx.h"
#include "inventory_system.h"
#include "item.h"
#include "container.h"
#include "Inventory.h"

#include <algorithm>

void Inventory_system::setup_events ()
{
	m_dispatchers.emplace ("dummy", Dispatcher{});
}

Dispatcher& Inventory_system::get_event (std::string event)
{
	return m_dispatchers["dummy"];
}

void Inventory_system::add_to_pack (Entity_id item, Entity_id target)
{
	if (!entity_mgr->has_component (item, Component::Item_shared)) return;
	auto shared_data = entity_mgr->get_component<item::Item_shared> (Component::Item_shared);
	auto weight = shared_data->get_data (item).m_weight;
	auto pack = entity_mgr->get_component<Container>(Component::Container)->get_data(target);
	if (weight < pack.capacity - pack.total_weight)
	{
		pack.contents.push_back (item);
		pack.total_weight += weight;
	}
}

void Inventory_system::drop (Entity_id item, Entity_id target)
{
	auto pack = entity_mgr->get_component<Container> (Component::Container)->get_data(target);
	auto weight = entity_mgr->get_component<item::Item_shared> (Component::Item_shared)->get_data(item).m_weight;
	auto pack_itr = std::find (pack.contents.cbegin (), pack.contents.cend (), item);
	if (pack_itr == pack.contents.cend ())
	{
		auto inv = entity_mgr->get_component<Inventory> (Component::Inventory)->get_data(target);
		auto inv_itr = std::find_if (inv.m_slots.cbegin (), inv.m_slots.cend (), [item](auto y) {return y.second == item; });
		if (inv_itr != inv.m_slots.cend ())
		{
			inv.m_slots.erase (inv_itr);
			inv.m_total_weight -= weight;
		}
	}
	else
	{
		pack.contents.erase (pack_itr);
		pack.total_weight -= weight;
	}
}*/