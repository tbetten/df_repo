#include "equippable.h"
#include "shared_context.h"
#include "db.h"

void Equippable::load (const std::string& key, Shared_context* context)
{
	//auto db = db::DB_connection { context->m_database_path };
	auto db = db::DB_connection::create (context->m_database_path);
	auto stmt = db->prepare ("select equip_slot, tilesheet, tile_index from equippable inner join tile_catalog on equippable.tilesheet = tile_catalog.sheet and equippable.tile_key = tile_catalog.key where equippable.key = ?");
	stmt.bind (1, key);
	auto result = stmt.execute_row ();
	if (result == db::Prepared_statement::Row_result::Row)
	{
		auto row = stmt.fetch_row ();
		tilesheet = std::get<std::string> (row ["tilesheet"]);
		tile_index = std::get<int> (row ["tile_index"]);
		auto n = std::get<std::string> (row ["equip_slot"]);
		auto itr = std::find_if (std::cbegin (inventory::string_to_slot), std::cend (inventory::string_to_slot), [n = std::get<std::string> (row ["equip_slot"])] (const inventory::slot& s){return n == s.first; });
		slot = itr->second;
		//slot = inventory::string_to_slot.at (std::get<std::string> (row ["equip_slot"]));
	}
}