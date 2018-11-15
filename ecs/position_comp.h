#pragma once
#include "ecs_base.h"
#include "hexlib.h"

struct Position_comp : public cloneable<Position_comp, Component_base>
{
	using Ptr = std::shared_ptr<Position_comp>;
	Position_comp () : map_id{ "" }, coords{ 0, 0 }, elevation{ 0 }{}
	static Ptr create () { return std::make_shared<Position_comp> (); }
	void load_from_db (db::db_connection* db, const std::string& key) override {}

	std::string map_id;
	hexlib::Hex coords;
	int elevation;
};