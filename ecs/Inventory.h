#pragma once

#include "ecs_base.h"

enum class Equipment_slot {Head, Torso, Hands, Feet, Necklace, Ring1, Ring2, Left_hand, Right_hand, Quiver};
struct Inventory_data
{
	std::unordered_map<Equipment_slot, Entity_id> m_slots;
	unsigned int m_total_weight = 0;
};

struct Inventory : public Component_base
{
	using Ptr = std::shared_ptr<Inventory>;
	static Ptr create () { return std::make_shared<Inventory> (); }
	void load_from_db (db::db_connection* db, const std::string& key) override {}
	void add_entity (Entity_id entity) override { m_data.emplace (entity, Inventory_data{}); }
	bool has_entity (Entity_id entity) override { return m_data.count (entity); }
	Inventory_data& get_data (Entity_id entity) { return m_data[entity]; }

//	unsigned int m_total_weight;

	std::unordered_map<Entity_id, Inventory_data> m_data;
//	std::unordered_map<Equipment_slot, Entity_id> m_slots;
};