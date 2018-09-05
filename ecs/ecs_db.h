#pragma once
#include "db.h"
#include "item.h"

class Ecs_db
{
public:
	Ecs_db();
	std::vector<std::string> load_entities();
	void load_components(const std::string& entity_key);
private:
	db::db_connection_ptr m_database = nullptr;
	db::prepared_statement_ptr m_select_entity = nullptr;
};