#include "resource_cache.h"
#include "db.h"
#include <variant>
#include <ostream>
#include <array>

namespace cache
{
	enum class Resource_type { Texture, Tileset, Font };  // must agree with table resource_type of database

	constexpr std::array<Pf, 3> funcs { &Texture_resource::load_resource, nullptr};

	void Cache::init ()
	{
		std::string sql = "select key, type, path from resource";
		//auto db = db::db_connection::create("assets/database/gamedat.db");
		auto db = db::DB_connection{ "assets/database/gamedat.db" };
		auto stmt = db.prepare(sql);
		auto data = stmt.fetch_table();
		for (auto row : data)
		{
			std::string key = std::get<std::string>(row["key"]);
			auto type = std::get<int>(row["type"]);
			std::string path = std::get<std::string>(row["path"]);
			loadfuncs[key] = funcs [type];
			paths[key] = path;
		}
	}

	Texture_resource::Resource (fs::path file)
	{
		if (!val.loadFromFile(file.string())) std::cout << "error reading texture " << file << "\n";
	}

/*	Tileset_resource::Resource (fs::path file)
	{
		val.loadFromFile (file.string ());
	}*/

	Font_resource::Resource(fs::path file)
	{
		val.loadFromFile(file.string());
	}
}