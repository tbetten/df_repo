#pragma once
#include "attributes.h"

#include <string>
#include <vector>

struct Skill_rec
{
	std::string key;
	int level;
	int points_spent;
};

struct Skill_comp
{
	void load (const std::string& key, Shared_context* context) {}
	void reset ();
	std::vector<Skill_rec> m_skills;
};