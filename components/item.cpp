#include "item.h"
#include "db.h"
#include "shared_context.h"

void Item::reset()
{
	item_name = "";
	description = "";
	weight = 0;
}

void Item::load (const std::string& key, Shared_context* context)
{
	auto database = db::DB_connection { context->m_database_path };
	auto stmt = database.prepare ("select name, description, weight, value, equippable, useable from equipment where key = ?");
	stmt.bind (1, key);
	auto res = stmt.execute_row ();
	if (res == db::Prepared_statement::Row_result::Row)
	{
		auto row = stmt.fetch_row ();
		item_name = std::get<std::string> (row ["name"]);
		description = std::get<std::string> (row ["description"]);
		weight = std::get<int> (row ["weight"]);
		value = std::get<int> (row ["value"]);
		equippable = std::get<int> (row ["equippable"]) != 0;
		useable = std::get<int> (row ["useable"]) != 0;
	}
	else
	{
		throw ("component data for component Item not found for key " + key);
	}
}