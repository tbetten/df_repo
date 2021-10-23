#include "armour.h"
#include "shared_context.h"
#include "resource_cache.h"
#include "Tilesheet.h"
#include "db.h"

#include <cassert>
#include <map>

/*Armour::Equip_slot convert_equip_slot (const std::string& slot)
{
	std::map<std::string, Armour::Equip_slot> slots { {"torso", Armour::Equip_slot::Torso} };
	return slots.at (slot);
}*/

void Armour::load (const std::string& key, Shared_context* context)
{
	//auto database = db::DB_connection { context->m_database_path };
	auto database = db::DB_connection::create (context->m_database_path);
	auto stmt = database->prepare ("select dr, dr_crushing, equip_slot, tilesheet, tile_index from armour inner join tile_catalog on armour.tilesheet = tile_catalog.sheet and armour.tile_key = tile_catalog.key where armour.key = ?");
	stmt.bind (1, key);
	auto res = stmt.execute_row ();
	assert (res == db::Prepared_statement::Row_result::Row);
	auto row = stmt.fetch_row ();
	dr = std::get<int> (row ["DR"]);
	crushing_dr = std::get<int> (row ["DR_crushing"]);
//	equip_slot = convert_equip_slot (std::get<std::string> (row ["equip_slot"]));
	tilesheet = std::get<std::string> (row ["tilesheet"]);
	tile_index = std::get<int> (row ["tile_index"]);
}

