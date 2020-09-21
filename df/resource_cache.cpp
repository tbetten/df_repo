#include "resource_cache.h"
#include "db.h"
//#include <variant>
#include <ostream>
#include <array>

namespace cache
{
	Cache::Cache ()
	{
		db::DB_connection db { "./assets/database/gamedat.db" };
		//auto db = db::DB_factory::create (R"("assets\database\gamedat.db")");
		auto sql = "select key, path from resource";
		auto stmt = db.prepare (sql);
		auto result = stmt.fetch_table ();
		m_cache.reserve (result.size ());
		for (auto row : result)
		{
			m_cache.emplace_back (std::get<std::string> (row ["key"]), std::get<std::string> (row ["path"]));
		}
	}

	bool Cache::load_cache_entry (const std::string& key)
	{
	//	auto db = db::DB_factory::create (R"("assets\database\gamedat.db")");
		db::DB_connection db { "./assets/database/gamedat.db" };
		auto sql = "select path from resource where key = ?";
		auto stmt = db.prepare (sql);
		stmt.bind (1, key);
		if (stmt.execute_row () != db::Prepared_statement::Row_result::Row) return false;
		auto row = stmt.fetch_row ();
		auto path = std::get<std::string> (row ["path"]);
		m_cache.emplace_back (key, path);
		return true;
	}
}