#pragma once
#include <string>
#include <vector>
#include <array>
#include "attributes.h"

namespace skill
{
	struct Default
	{
		std::string skill;
		int modifier;
	};

	struct Skill
	{
		enum class Difficulty { Invalid = -1, NA, Easy, Average, Hard, Very_hard };
		enum class Skill_type { Skill, Attribute };
		std::string key;
		std::string name;
		attributes::Attrib attribute;
		Difficulty difficulty;
		Skill_type type;
		std::vector<Default> defaults;
	};

	

	Skill::Difficulty string_to_difficulty (const std::string& s);

	std::vector<Skill> load_skills (std::string db_name);
	std::string print_skill (Skill& s);
}