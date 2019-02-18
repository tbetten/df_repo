#pragma once

#include "system.h"
#include "Inventory.h"

class Inventory_system : public System_base
{
public:
	using Ptr = std::unique_ptr<Inventory_system>;
	explicit Inventory_system (System_manager* mgr) : System_base (System::Inventory, mgr)
	{
		entity_mgr = m_system_manager->get_entity_mgr ();
		m_inventory_comp = entity_mgr->get_component<Inventory> (Component::Inventory);
		Bitmask b;
		b.set (to_number (Component::Inventory));
		b.set (to_number (Component::Container));
		m_required_components.push_back(b);
	}
	static Ptr create (System_manager* mgr) { return std::unique_ptr< Inventory_system> (); }
	void setup_events () override;
	void update (int dt) override {}
	Dispatcher& get_event (std::string event) override;

	void add_to_pack (Entity_id item, Entity_id target);
	void drop (Entity_id item, Entity_id target);
	void equip (Entity_id entity, Equipment_slot slot);
	void unequip (Entity_id entity);
private:

	const std::vector<std::string> m_eventnames;
	std::unordered_map<std::string, Dispatcher> m_dispatchers;
	Entity_manager* entity_mgr;
	Inventory::Ptr m_inventory_comp;
};