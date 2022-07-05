#pragma once
#include "attributes.h"

#include <string>
#include <vector>
#include <compare>

namespace skill
{
	enum class Transaction_type { Buy, Equipment, Effect };
	enum class Template_type { None, Race, Occupation };
	enum class Difficulty { Easy, Average, Hard, Very_hard };

	struct Transaction
	{
		Transaction_type type;
		Template_type template_type;
		std::string skill;
		int amount;
		int points_spent;
	};

/*	auto operator<=> (const Transaction& t1, const Transaction& t2)
	{
		return t1.skill <=> t2.skill;
	}*/

	struct Skill_rec
	{
		std::string key;
		int level;
		int points_spent;
	};

	struct Skill_comp
	{
		void load(const std::string& key, Shared_context* context) {}
		void reset();
		//std::vector<Skill_rec> m_skills;
		std::vector<Transaction> m_skills;
	};
}