#include "Skill.h"
#include "db.h"

#include <optional>
#include <sstream>

namespace skill
{
	std::array<std::pair<std::string, Skill::Difficulty>, 5> string_to_diff { {{"NA", Skill::Difficulty::NA }, { "Easy", Skill::Difficulty::Easy }, { "Average", Skill::Difficulty::Average }, {"Hard", Skill::Difficulty::Hard}, {"Very_hard", Skill::Difficulty::Very_hard}} };

	Skill::Difficulty string_to_difficulty (const std::string& key)
	{
		auto itr = std::find_if (std::cbegin (string_to_diff), std::cend (string_to_diff), [key] (const std::pair<std::string, Skill::Difficulty>& p){return p.first == key; });
		if (itr == std::cend (string_to_diff)) return Skill::Difficulty::Invalid;
		return itr->second;
	}

	std::optional<Default> make_default (db::row_t& row)
	{
		if (std::holds_alternative<std::string> (row ["default_skill"]))
		{
			std::string default_skill = std::get<std::string> (row ["default_skill"]);
			int modifier = std::holds_alternative<int> (row ["modifier"]) ? std::get<int> (row ["modifier"]) : 0;
			return Default { default_skill, modifier };
		}
		return std::nullopt;
	}

	std::vector<Skill> load_skills (std::string db_name)
	{
		std::vector<Skill> result {};
		std::string sql = "select s.key, s.name, s.difficulty, s.attribute, d.default_skill, d.modifier from skill s left outer join skill_default d on s.key = d.skill";
		//auto db = db::DB_connection { db_name };
		auto db = db::DB_connection::create (db_name);
		auto stmt = db->prepare (sql);
		auto table = stmt.fetch_table ();
		for (auto& row : table)
		{
			auto& key = std::get<std::string> (row ["key"]);
			auto default_opt = make_default (row);
			auto key_itr = std::find_if (std::begin (result), std::end (result), [&key] (const Skill& s){return s.key == key; });
			if (key_itr == std::end (result))
			{
				
				auto& name = std::get<std::string> (row ["name"]);
				auto& diff = std::get<std::string> (row ["difficulty"]);
				auto difficulty = string_to_difficulty (diff);
				if (difficulty == Skill::Difficulty::Invalid) continue;
				auto& attr = std::get<std::string> (row ["attribute"]);
				auto attribute = attributes::string_to_attrib (attr);
				Skill::Skill_type type = difficulty == Skill::Difficulty::NA ? Skill::Skill_type::Attribute : Skill::Skill_type::Skill;
				std::vector<Default> defaults {};
				if (default_opt)
				{
					defaults.push_back (default_opt.value ());
				}
				result.emplace_back (key, name, attribute, difficulty, type, defaults);
			}
			else
			{
				if (default_opt)
				{
					key_itr->defaults.push_back (default_opt.value ());
				}
			}
		}
		return result;
	}

	std::string print_skill (Skill& s)
	{
		const std::string sep = "\t";
		std::ostringstream ss;
		ss << s.key << sep << s.name << sep << attributes::attrib_to_string(s.attribute) << sep << static_cast<int>(s.difficulty) << "\n";
		for (auto& def : s.defaults)
		{
			ss << sep << def.skill << sep << def.modifier << "\n";
		}
		return ss.str ();
	}
}