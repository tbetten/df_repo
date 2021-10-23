#pragma once

#include "db.h"
#include <string>
#include <vector>
#include <tuple>
#include <optional>

class DB_queries
{
public:
	static int query_tile_index(std::string tilesheet, std::string tile);
	static std::vector<std::tuple<std::string, int>> query_tiles(std::string tilesheet);
	static int query_charpoints();
	static std::tuple<int, std::string> query_icon_key(std::string race, std::string gender);
	static std::tuple<std::string, std::string, int, int>query_equipment(std::string key);
	static std::vector<std::tuple<std::string, std::string, std::string, int, int, int>> get_racial_talents();
	static std::vector<std::string> get_skills_by_talent(std::string talent);
	static std::vector<std::tuple<std::string, std::string, std::string, int, int, int, int, std::optional<int>>> get_racial_advantages();
private:
	inline static db::DB_connection::Ptr connection = db::DB_connection::create ("./assets/database/gamedat.db");//{ "./assets/database/gamedat.db" };
};