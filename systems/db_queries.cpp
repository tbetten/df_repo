#include "db_queries.h"
#include <algorithm>

int DB_queries::query_tile_index(std::string tilesheet, std::string tile)
{
	auto stmt = connection.prepare("select tile_index from tile_catalog where sheet = ? and key = ?");
	stmt.bind(1, std::move(tilesheet));
	stmt.bind(2, std::move(tile));
	stmt.execute_row();
	auto row = stmt.fetch_row();
	return std::get<int>(row["tile_index"]);
}

std::vector<std::tuple<std::string, int>> DB_queries::query_tiles(std::string tilesheet)
{
	auto stmt = connection.prepare("select key, tile_index from tile_catalog where sheet = ?");
	stmt.bind(1, std::move(tilesheet));
	auto table = stmt.fetch_table();
	std::vector<std::tuple<std::string, int>> res{};
	std::transform(std::cbegin(table), std::cend(table), std::back_inserter(res), [](const db::row_t& row) {return std::make_tuple(std::get<std::string>(row.at("key")), std::get<int>(row.at("tile_index"))); });
	return res;
}

int DB_queries::query_charpoints()
{
	auto stmt = connection.prepare("select charpoints from gamesettings");
	stmt.execute_row();
	auto row = stmt.fetch_row();
	return std::get<int>(row["charpoints"]);
}

std::tuple<int, std::string> DB_queries::query_icon_key(std::string race, std::string gender)
{
	auto stmt = connection.prepare("select tile_index, sheet from tile_catalog inner join player_icons on tile_catalog.key = player_icons.icon_key and tile_catalog.sheet = player_icons.tilesheet where player_icons.race = ? and player_icons.gender = ?; ");
	stmt.bind(1, std::move(race));
	stmt.bind(2, std::move(gender));
	stmt.execute_row();
	auto row = stmt.fetch_row();
	return std::make_tuple(std::get<int>(row["tile_index"]), std::get<std::string>(row["sheet"]));
}

std::tuple<std::string, std::string, int, int> DB_queries::query_equipment(std::string key)
{
	auto stmt = connection.prepare("select name, description, weight, value from equipment where key = ?");
	stmt.bind(1, std::move(key));
	stmt.execute_row();
	auto row = stmt.fetch_row();
	return std::make_tuple(std::get<std::string>(row["name"]), std::get<std::string>(row["description"]), std::get<int>(row["weight"]), std::get<int>(row["value"]));
}

std::vector<std::tuple<std::string, std::string, std::string, int, int, int>> DB_queries::get_racial_talents()
{
	auto stmt = connection.prepare("select race, name, description, max_levels, standard_level, cost_per_level from racial_talent");
	auto table = stmt.fetch_table();
	std::vector<std::tuple<std::string, std::string, std::string, int, int, int>> res{};
	std::transform(std::cbegin(table), std::cend(table), std::back_inserter(res), [](const db::row_t& row) {return std::make_tuple(std::get<std::string>(row.at("race")), std::get<std::string>(row.at("name")), std::get<std::string>(row.at("description")), std::get<int>(row.at("max_levels")), std::get<int>(row.at("standard_level")), std::get<int>(row.at("cost_per_level"))); });
	return res;
}

std::vector<std::string> DB_queries::get_skills_by_talent(std::string talent)
{
	auto stmt = connection.prepare("select skill from talent_skill where talent = ?");
	stmt.bind(1, std::move(talent));
	auto table = stmt.fetch_table();
	std::vector<std::string> res{};
	std::transform(std::cbegin(table), std::cend(table), std::back_inserter(res), [](const db::row_t& row) {return std::get<std::string>(row.at("skill")); });
	return res;
}

std::vector<std::tuple<std::string, std::string, std::string, int, int, int, int, std::optional<int>>> DB_queries::get_racial_advantages()
{
	auto stmt = connection.prepare("select advantage, description, race, default_level, max_level, cost_per_level, uses_self_control, self_control from advantage_template join advantage on advantage_template.advantage = advantage.name where race is not null");
	auto table = stmt.fetch_table();
	std::vector<std::tuple<std::string, std::string, std::string, int, int, int, int, std::optional<int>>> res{};
	std::transform(std::cbegin(table), std::cend(table), std::back_inserter(res), [](const db::row_t& row) 
		{
			std::optional<int> selfcontrol;
			if (std::holds_alternative<std::monostate>(row.at("self_control")))
			{
				selfcontrol = std::nullopt;
			}
			else
			{
				selfcontrol = std::get<int>( row.at("self_control"));
			}
			return std::make_tuple(std::get<std::string>(row.at("advantage")), std::get<std::string>(row.at("description")), std::get<std::string>(row.at("race")), std::get<int>(row.at("default_level")), std::get<int>(row.at("max_level")), std::get<int>(row.at("cost_per_level")), std::get<int>(row.at("uses_self_control")), selfcontrol); 
		});
	return res;
}