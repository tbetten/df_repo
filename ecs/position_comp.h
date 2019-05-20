/*#pragma once
#include "ecs_base.h"
#include "hexlib.h"

struct Position_data
{
	std::string map_id;
	hexlib::Hex coords;
	int elevation = 0;
};

struct Position_comp : public Component_base
{
	using Ptr = std::shared_ptr<Position_comp>;
	//Position_comp () : map_id{ "" }, coords{ 0, 0 }, elevation{ 0 }{}
	static Ptr create () { return std::make_shared<Position_comp> (); }
	void load_from_db (db::db_connection* db, const std::string& key) override {}
	void add_entity (Entity_id entity) override { m_data.emplace (entity, Position_data{}); }
	bool has_entity (Entity_id entity) override { return m_data.count (entity); }
	Position_data& get_data (Entity_id entity) { return m_data[entity]; }

	std::unordered_map<Entity_id, Position_data> m_data;
};*/