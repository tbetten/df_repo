#include "stdafx.h"
#include "ecs_db.h"

#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;
Ecs_db::Ecs_db()
{
	fs::path dbasepath;
	try
	{
		dbasepath = fs::canonical(fs::current_path() / "../db/dfdata.db");
	}
	catch (fs::filesystem_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	if (!fs::is_regular_file(dbasepath))
	{
		std::cout << "database niet gevonden op pad " << dbasepath << std::endl;
	}
	m_database = db::db_connection::create(dbasepath.string());
	if (m_database != nullptr)
	{
		std::cout << "verbonden met database" << std::endl;
	}
}

std::vector<std::string> Ecs_db::load_entities()
{
	std::vector<std::string> result;
	if (m_select_entity == nullptr)
	{
		std::string select_entity_sql = "select key from entity";
		m_select_entity = m_database->prepare(select_entity_sql);
	}
	auto entity_data = m_select_entity->fetch_table();
	for (auto row : entity_data)
	{
		result.emplace_back(row["key"].string_value);
	}
	return result;
}

void Ecs_db::load_components(const std::string& entity_key)
{
	using string_val = std::pair<std::string, std::string>;
	using int_val = std::pair<std::string, int>;
	using float_val = std::pair<std::string, float>;
	using Type = db::value::Type;

	std::string sql = "select * from item_shared where entity_key = ?";
	auto query = m_database->prepare(sql);
	query->bind(1, entity_key);
	auto data = query->fetch_table();
	for (auto row : data)
	{
		for (auto entry : row)
		{
			auto colname = entry.first;
			auto type = entry.second.type;
			if (type == db::value::Type::STRING)
			{
				std::cout << "column " << colname << " value " << entry.second.string_value << std::endl;
			}
		}
		std::cout << row["description"].string_value << std::endl;
	}
}