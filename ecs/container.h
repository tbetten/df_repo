#pragma once

#include "ecs_base.h"

struct Container_data
{
	unsigned int total_weight = 0;
	unsigned int empty_weight = 0;
	unsigned int capacity = 0;
	std::vector<Entity_id> contents;
};

struct Container : public Component_base
{
	using Ptr = std::shared_ptr<Container>;
	static Ptr create () { return std::make_shared<Container> (); }
	void load_from_db (db::db_connection* db, const std::string& key) override {}
	void add_entity (Entity_id entity) override { m_data.emplace (entity, Container_data{}); }
	bool has_entity (Entity_id entity) override { return m_data.count (entity); }
	Container_data& get_data (Entity_id entity) { return m_data[entity]; }

	std::unordered_map<Entity_id, Container_data> m_data;
//	unsigned int total_weight;
//	unsigned int empty_weight;
//	unsigned int capacity;
//	std::vector<Entity_id> contents;
};